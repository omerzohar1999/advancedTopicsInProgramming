cmake_minimum_required(VERSION 3.16)
project(Algorithm2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include common and algo_common directories
include_directories(${CMAKE_SOURCE_DIR}/common ${CMAKE_SOURCE_DIR}/algo_common)

# Add the source files for Algorithm2
add_library(algorithm2 SHARED Algorithm2.cpp ${CMAKE_SOURCE_DIR}/algo_common/AlgorithmGraph.cpp)

# Set the output name for the shared library
set_target_properties(algorithm2 PROPERTIES OUTPUT_NAME "libAlgorithm2")
