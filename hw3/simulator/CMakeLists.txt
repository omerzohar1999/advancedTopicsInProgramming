cmake_minimum_required(VERSION 3.16)
project(Simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include common directory
include_directories(${CMAKE_SOURCE_DIR}/common)

# Add the source files for the simulator
add_executable(simulator main.cpp)
