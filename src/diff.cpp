#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <regex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct DiffConfig {
    bool ignoreArrayOrder = false;
    bool ignoreNumericDiffsLessThan = false;
    double numericTolerance = 0.01;
    std::vector<std::string> ignorePaths;
    std::vector<std::regex> ignoreRegexes;
    bool printOnlyDiffs = true;
    bool colorOutput = true;
    bool prettyPrintValues = true;
    bool summarizeArrays = true;
    int maxArrayDiffsToShow = 5;
};

struct DiffStats {
    int typeMismatches = 0;
    int valueDifferences = 0;
    int missingKeys1 = 0;
    int missingKeys2 = 0;
    int arraySizeDifferences = 0;
    int arrayElementDifferences = 0;
    int totalFields = 0;
    int comparedFields = 0;

    int getTotalDiffs() const {
        return typeMismatches + valueDifferences +
               missingKeys1 + missingKeys2 +
               arraySizeDifferences + arrayElementDifferences;
    }
};

// ANSI color codes
const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_BLUE = "\033[34m";
const std::string COLOR_MAGENTA = "\033[35m";

// Helper function to check if a path should be ignored
bool shouldIgnorePath(const std::string& path, const DiffConfig& config) {
    for (const auto& ignorePath : config.ignorePaths) {
        if (path == ignorePath) return true;

        // Check for wildcard patterns like "results.*.timestamp"
        std::regex wildcard(
            "^" + std::regex_replace(ignorePath, std::regex("\\.\\*\\."), "\\.[^.]+\\.") + "$"
        );
        if (std::regex_match(path, wildcard)) return true;
    }

    for (const auto& regex : config.ignoreRegexes) {
        if (std::regex_match(path, regex)) return true;
    }

    return false;
}

// Helper function to colorize output
std::string colorize(const std::string& text, const std::string& color, const DiffConfig& config) {
    if (config.colorOutput) {
        return color + text + COLOR_RESET;
    }
    return text;
}

// Function to pretty print JSON values
std::string prettyPrintValue(const json& value, const DiffConfig& config) {
    if (!config.prettyPrintValues) {
        return value.dump();
    }

    if (value.is_string()) {
        return "\"" + value.get<std::string>() + "\"";
    } else if (value.is_array() && config.summarizeArrays && value.size() > 3) {
        return "[Array with " + std::to_string(value.size()) + " elements]";
    } else if (value.is_object() && value.size() > 3) {
        return "{Object with " + std::to_string(value.size()) + " fields}";
    } else {
        return value.dump();
    }
}

json loadJsonFromFile(const std::string& filename) {
    std::ifstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    json jsonData;
    try {
        jsonFile >> jsonData;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("Failed to parse JSON in file: " + filename +
                                 " - " + e.what());
    }
    return jsonData;
}

void compareArrays(const json& arr1, const json& arr2, const std::string& path,
                  DiffConfig& config, DiffStats& stats) {
    if (arr1.size() != arr2.size()) {
        std::cout << colorize("Array size difference at " + path +
                              ": " + std::to_string(arr1.size()) +
                              " vs " + std::to_string(arr2.size()),
                             COLOR_YELLOW, config) << std::endl;
        stats.arraySizeDifferences++;
    }

    if (config.ignoreArrayOrder) {
        std::set<std::string> items1, items2;
        for (const auto& item : arr1) {
            if (item.is_primitive()) {
                items1.insert(item.dump());
            }
        }
        for (const auto& item : arr2) {
            if (item.is_primitive()) {
                items2.insert(item.dump());
            }
        }

        for (const auto& item : items1) {
            if (items2.find(item) == items2.end()) {
                std::cout << colorize("Item only in first array at " + path +
                                      ": " + item, COLOR_BLUE, config) << std::endl;
                stats.arrayElementDifferences++;
            }
        }

        for (const auto& item : items2) {
            if (items1.find(item) == items1.end()) {
                std::cout << colorize("Item only in second array at " + path +
                                      ": " + item, COLOR_BLUE, config) << std::endl;
                stats.arrayElementDifferences++;
            }
        }
    } else {
        // Element-by-element comparison
        int diffCount = 0;
        for (size_t i = 0; i < std::min(arr1.size(), arr2.size()); ++i) {
            if (diffCount < config.maxArrayDiffsToShow) {
                std::string itemPath = path + "[" + std::to_string(i) + "]";
                if (!shouldIgnorePath(itemPath, config)) {
                    findDifferences(arr1[i], arr2[i], itemPath, config, stats);
                }
            }
            diffCount++;
        }

        if (diffCount > config.maxArrayDiffsToShow) {
            std::cout << colorize("... and " +
                                  std::to_string(diffCount - config.maxArrayDiffsToShow) +
                                  " more array element differences at " + path,
                                 COLOR_MAGENTA, config) << std::endl;
        }
    }
}

// Main comparison function with enhanced features
void findDifferences(const json& json1, const json& json2, const std::string& path,
                    DiffConfig& config, DiffStats& stats) {
    stats.totalFields++;

    if (shouldIgnorePath(path, config)) {
        return;
    }

    stats.comparedFields++;

    if (json1.is_array() && json2.is_array()) {
        compareArrays(json1, json2, path, config, stats);
        return;
    }

    if (json1.is_object()) {
        for (auto it = json1.begin(); it != json1.end(); ++it) {
            std::string currentPath = path.empty() ? it.key() : path + "." + it.key();

            if (shouldIgnorePath(currentPath, config)) {
                continue;
            }

            if (json2.contains(it.key())) {
                if (it.value().type() != json2[it.key()].type()) {
                    std::cout << colorize("Type mismatch at " + currentPath +
                                          ": " + std::string(it.value().type_name()) +
                                          " vs " + std::string(json2[it.key()].type_name()),
                                         COLOR_RED, config) << std::endl;
                    stats.typeMismatches++;
                } else if (it.value().is_structured()) {
                    findDifferences(it.value(), json2[it.key()], currentPath, config, stats);
                } else if (it.value().is_number() && json2[it.key()].is_number() &&
                          config.ignoreNumericDiffsLessThan) {
                    double val1 = it.value().is_number_float() ?
                                  it.value().get<double>() :
                                  static_cast<double>(it.value().get<int64_t>());

                    double val2 = json2[it.key()].is_number_float() ?
                                  json2[it.key()].get<double>() :
                                  static_cast<double>(json2[it.key()].get<int64_t>());

                    if (std::abs(val1 - val2) > config.numericTolerance) {
                        std::cout << colorize("Numeric difference at " + currentPath +
                                              ": " + std::to_string(val1) +
                                              " vs " + std::to_string(val2),
                                             COLOR_YELLOW, config) << std::endl;
                        stats.valueDifferences++;
                    }
                } else if (it.value() != json2[it.key()]) {
                    std::cout << colorize("Value difference at " + currentPath + ": " +
                                          prettyPrintValue(it.value(), config) +
                                          " vs " +
                                          prettyPrintValue(json2[it.key()], config),
                                         COLOR_YELLOW, config) << std::endl;
                    stats.valueDifferences++;
                }
            } else {
                std::cout << colorize("Key " + currentPath +
                                      " is missing in second JSON",
                                     COLOR_BLUE, config) << std::endl;
                stats.missingKeys2++;
            }
        }

        for (auto it = json2.begin(); it != json2.end(); ++it) {
            std::string currentPath = path.empty() ? it.key() : path + "." + it.key();

            if (shouldIgnorePath(currentPath, config)) {
                continue;
            }

            if (!json1.contains(it.key())) {
                std::cout << colorize("Key " + currentPath +
                                      " is missing in first JSON",
                                     COLOR_GREEN, config) << std::endl;
                stats.missingKeys1++;
            }
        }
    } else if (json1 != json2) {
        std::cout << colorize("Value difference at " + path + ": " +
                             prettyPrintValue(json1, config) +
                             " vs " +
                             prettyPrintValue(json2, config),
                            COLOR_YELLOW, config) << std::endl;
        stats.valueDifferences++;
    }
}

void printDiffStats(const DiffStats& stats, const DiffConfig& config);

void parseArgs(int argc, char* argv[], DiffConfig& config,
               std::string& file1, std::string& file2) {
    file1 = "";
    file2 = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--ignore-order") {
            config.ignoreArrayOrder = true;
        } else if (arg == "--ignore-path" && i + 1 < argc) {
            config.ignorePaths.push_back(argv[++i]);
        } else if (arg == "--ignore-regex" && i + 1 < argc) {
            config.ignoreRegexes.push_back(std::regex(argv[++i]));
        } else if (arg == "--numeric-tolerance" && i + 1 < argc) {
            config.ignoreNumericDiffsLessThan = true;
            config.numericTolerance = std::stod(argv[++i]);
        } else if (arg == "--no-color") {
            config.colorOutput = false;
        } else if (arg == "--show-all") {
            config.printOnlyDiffs = false;
        } else if (arg == "--max-array-diffs" && i + 1 < argc) {
            config.maxArrayDiffsToShow = std::stoi(argv[++i]);
        } else if (arg.rfind("--", 0) != 0) {
            if (file1.empty()) {
                file1 = arg;
            } else if (file2.empty()) {
                file2 = arg;
            }
        }
    }

    if (file1.empty() || file2.empty()) {
        throw std::runtime_error("Two JSON files must be specified.");
    }
}

void printDiffStats(const DiffStats& stats, const DiffConfig& config) {
    std::cout << "\n--- Comparison Summary ---" << std::endl;
    std::cout << "Total fields: " << stats.totalFields << std::endl;
    std::cout << "Compared fields: " << stats.comparedFields
              << " (" << stats.totalFields - stats.comparedFields << " ignored)" << std::endl;

    if (stats.getTotalDiffs() == 0) {
        std::cout << colorize("No differences found! JSON files are " +
                               (stats.totalFields == stats.comparedFields ?
                                "identical" : "equivalent (ignoring specified paths)"),
                              COLOR_GREEN, config) << std::endl;
    } else {
        std::cout << colorize("Found " + std::to_string(stats.getTotalDiffs()) +
                              " differences:", COLOR_RED, config) << std::endl;

        if (stats.typeMismatches > 0) {
            std::cout << "- Type mismatches: " << stats.typeMismatches << std::endl;
        }
        if (stats.valueDifferences > 0) {
            std::cout << "- Value differences: " << stats.valueDifferences << std::endl;
        }
        if (stats.missingKeys1 > 0) {
            std::cout << "- Keys missing in first file: " << stats.missingKeys1 << std::endl;
        }
        if (stats.missingKeys2 > 0) {
            std::cout << "- Keys missing in second file: " << stats.missingKeys2 << std::endl;
        }
        if (stats.arraySizeDifferences > 0) {
            std::cout << "- Array size differences: " << stats.arraySizeDifferences << std::endl;
        }
        if (stats.arrayElementDifferences > 0) {
            std::cout << "- Array element differences: " << stats.arrayElementDifferences << std::endl;
        }
    }
}
