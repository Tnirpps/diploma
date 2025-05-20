#include <cstdint>
#include <iterator>
#include <string_view>

#include <benchmark/benchmark.h>
#include <userver/engine/run_standalone.hpp>

void Benchmark(benchmark::State& state) {
  userver::engine::RunStandalone([&] {
    constexpr std::string_view kNames[] = {"check diff for results"};
    std::uint64_t i = 0;

    for (auto _ : state) {
      const auto name = kNames[i++ % std::size(kNames)];
      auto result = search_market_place::diff(name);
      benchmark::DoNotOptimize(result);
    }
  });
}

BENCHMARK(Benchmark);
