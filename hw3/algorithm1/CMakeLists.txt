cmake_minimum_required(VERSION 3.16)
project(Algorithm1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include common and algo_common directories
include_directories(${CMAKE_SOURCE_DIR}/common ${CMAKE_SOURCE_DIR}/algo_common)

# Add the source files for Algorithm1
add_library(algorithm1 SHARED Algorithm1.cpp ${CMAKE_SOURCE_DIR}/algo_common/AlgorithmGraph.cpp)

# Set the output name for the shared library
set_target_properties(algorithm1 PROPERTIES OUTPUT_NAME "libAlgorithm1")
