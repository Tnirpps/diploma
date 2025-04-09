cmake_minimum_required(VERSION 3.12...3.31)
project(post_search CXX)




function(userver_target_generate_chaotic TARGET)
  set(OPTIONS GENERATE_SERIALIZERS PARSE_EXTRA_FORMATS)
  set(ONE_VALUE_ARGS OUTPUT_DIR RELATIVE_TO FORMAT INSTALL_INCLUDES_COMPONENT OUTPUT_PREFIX ERASE_PATH_PREFIX)
  set(MULTI_VALUE_ARGS SCHEMAS LAYOUT INCLUDE_DIRS)
  cmake_parse_arguments(
      PARSE "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN}
  )

  get_property(CHAOTIC_BIN GLOBAL PROPERTY userver_chaotic_bin)
  get_property(CHAOTIC_EXTRA_ARGS GLOBAL PROPERTY userver_chaotic_extra_args)
  get_property(USERVER_CHAOTIC_PYTHON_BINARY GLOBAL PROPERTY userver_chaotic_python_binary)
  get_property(CLANG_FORMAT_BIN GLOBAL PROPERTY userver_clang_format_bin)

  if (NOT DEFINED PARSE_OUTPUT_DIR)
    set(PARSE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  endif()
  file(MAKE_DIRECTORY "${PARSE_OUTPUT_DIR}")

  if (NOT PARSE_SCHEMAS)
    message(FATAL_ERROR "SCHEMAS is required")
  endif()

  if (NOT PARSE_RELATIVE_TO)
    message(FATAL_ERROR "RELATIVE_TO is required")
  endif()

  if (PARSE_FORMAT OR (NOT DEFINED PARSE_FORMAT AND USERVER_CHAOTIC_FORMAT))
    if (NOT CLANG_FORMAT_BIN)
      message(FATAL_ERROR "clang-format is not found and FORMAT option is set. Please install clang-format.")
    endif()
    set(CLANG_FORMAT "${CLANG_FORMAT_BIN}")
  else()
    set(CLANG_FORMAT "")
  endif()

  set(SCHEMAS)
  foreach(PARSE_SCHEMA ${PARSE_SCHEMAS})
    file(RELATIVE_PATH SCHEMA "${PARSE_RELATIVE_TO}" "${PARSE_SCHEMA}")

    string(REGEX REPLACE "^(.*)\\.([^.]*)\$" "\\1" SCHEMA "${SCHEMA}")

    list(APPEND SCHEMAS
	    "${PARSE_OUTPUT_DIR}/${PARSE_OUTPUT_PREFIX}/${SCHEMA}.cpp"
	    "${PARSE_OUTPUT_DIR}/${PARSE_OUTPUT_PREFIX}/${SCHEMA}.hpp"
	    "${PARSE_OUTPUT_DIR}/${PARSE_OUTPUT_PREFIX}/${SCHEMA}_fwd.hpp"
	    "${PARSE_OUTPUT_DIR}/${PARSE_OUTPUT_PREFIX}/${SCHEMA}_parsers.ipp"
    )
  endforeach()

  set(CHAOTIC_ARGS)

  foreach(MAP_ITEM ${PARSE_LAYOUT})
    list(APPEND CHAOTIC_ARGS "-n" "${MAP_ITEM}")
  endforeach()

  foreach(INCLUDE_DIR ${PARSE_INCLUDE_DIRS})
    list(APPEND CHAOTIC_ARGS "-I" "${INCLUDE_DIR}")
  endforeach()

  if (PARSE_GENERATE_SERIALIZERS)
    list(APPEND CHAOTIC_ARGS "--generate-serializers")
  endif()

  if (PARSE_PARSE_EXTRA_FORMATS)
    list(APPEND CHAOTIC_ARGS "--parse-extra-formats")
  endif()

  if (PARSE_ERASE_PATH_PREFIX)
    list(APPEND CHAOTIC_ARGS "-e" "${PARSE_ERASE_PATH_PREFIX}")
  endif()

  list(APPEND CHAOTIC_ARGS "-o" "${PARSE_OUTPUT_DIR}/${PARSE_OUTPUT_PREFIX}")
  list(APPEND CHAOTIC_ARGS "--relative-to" "${PARSE_RELATIVE_TO}")
  list(APPEND CHAOTIC_ARGS "--clang-format" "${CLANG_FORMAT}")

  _userver_initialize_codegen_flag()
  add_custom_command(
      OUTPUT
          ${SCHEMAS}
      COMMAND
          ${CMAKE_COMMAND} -E env
          "USERVER_PYTHON=${USERVER_CHAOTIC_PYTHON_BINARY}"
          "${CHAOTIC_BIN}"
          ${CHAOTIC_EXTRA_ARGS}
          ${CHAOTIC_ARGS}
          ${PARSE_SCHEMAS}
      DEPENDS
          ${PARSE_SCHEMAS}
      WORKING_DIRECTORY
          "${CMAKE_CURRENT_SOURCE_DIR}"
      VERBATIM
          ${CODEGEN}
  )
  _userver_codegen_register_files("${SCHEMAS}")
  add_library("${TARGET}" ${SCHEMAS})
  target_include_directories("${TARGET}" PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>")
  target_include_directories("${TARGET}" PUBLIC "$<BUILD_INTERFACE:${PARSE_OUTPUT_DIR}>")

  if(PARSE_INSTALL_INCLUDES_COMPONENT)
    foreach(FILE ${SCHEMAS})
      string(REGEX REPLACE "^(.*)\\.([^.]*)\$" "\\2" SUFFIX "${FILE}")
      if(SUFFIX STREQUAL cpp)
        continue()
      endif()
      
      _userver_directory_install(
          COMPONENT ${PARSE_INSTALL_INCLUDES_COMPONENT}
          FILES "${FILE}"
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PARSE_OUTPUT_PREFIX}"
      )
    endforeach()
  endif()
endfunction()



list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(userver COMPONENTS
    core
    grpc
    QUIET
)
if(NOT userver_FOUND)
  # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
  download_userver(TRY_DIR third_party/userver)
endif()

userver_setup_environment()


# Common sources
include_directories(src)

add_library(${PROJECT_NAME}_objs OBJECT
    src/greeting.cpp
    src/hello.cpp
    src/hello_grpc.cpp
)
target_link_libraries(${PROJECT_NAME}_objs
PUBLIC userver::core
PUBLIC userver::grpc
)


# Create a proto library with userver extensions
userver_add_grpc_library(${PROJECT_NAME}_proto PROTOS handlers/hello.proto)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC ${PROJECT_NAME}_proto)

# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
add_executable(${PROJECT_NAME}_unittest
    src/greeting_test.cpp
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Benchmarks
add_executable(${PROJECT_NAME}_benchmark
    src/greeting_benchmark.cpp
)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
add_google_benchmark_tests(${PROJECT_NAME}_benchmark)

# Functional testing
userver_testsuite_add_simple()

# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})



file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/schemas/*.yaml)
userver_target_generate_chaotic(${PROJECT_NAME}-chgen
    # Generate serializers for responses
    GENERATE_SERIALIZERS
    # Map '/components/schemas/*' JSONSchema types to C++ types in 'samples::hello' namespace
    LAYOUT
        "/components/schemas/([^/]*)/=samples::hello::{0}"
    OUTPUT_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/src
    SCHEMAS
        ${SCHEMAS}
    RELATIVE_TO
        ${CMAKE_CURRENT_SOURCE_DIR}
)
# target_link_libraries(${PROJECT_NAME}_objs PUBLIC ${PROJECT_NAME}-chgen)
