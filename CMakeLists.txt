cmake_minimum_required(VERSION 3.8)
project(cpp)

include(GoogleTest)

# Make testing available
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# External dependencies
add_subdirectory(dependencies)

# Internal projects
add_subdirectory(mystring)
add_subdirectory(printers)
add_subdirectory(array_vs_map)
add_subdirectory(http_json)
