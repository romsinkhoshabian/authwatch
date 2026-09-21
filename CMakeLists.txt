cmake_minimum_required(VERSION 3.16)
project(authwatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(authwatch_core src/log_parser.cpp src/detector.cpp)
target_include_directories(authwatch_core PUBLIC src)
target_compile_options(authwatch_core PUBLIC -Wall -Wextra -Wpedantic)

add_executable(authwatch src/main.cpp)
target_link_libraries(authwatch PRIVATE authwatch_core)

# --- Unit tests (GoogleTest, fetched at configure time) ---
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(authwatch_tests tests/test_parser.cpp tests/test_detector.cpp)
target_link_libraries(authwatch_tests PRIVATE authwatch_core GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(authwatch_tests)
