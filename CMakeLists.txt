cmake_minimum_required(VERSION 3.16)
project(authwatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(authwatch_core src/log_parser.cpp)
target_include_directories(authwatch_core PUBLIC src)
target_compile_options(authwatch_core PUBLIC -Wall -Wextra -Wpedantic)

add_executable(authwatch src/main.cpp)
target_link_libraries(authwatch PRIVATE authwatch_core)
