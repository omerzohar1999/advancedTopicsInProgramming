cmake_minimum_required(VERSION 3.13)

project(registration_example VERSION 1.0 LANGUAGES CXX)
#set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")


add_library(algorithm1 SHARED
algorithm1/Algorithm1.cpp algo_common/BaseAlgorithm.cpp algo_common/AlgorithmGraph.cpp)

target_include_directories(algorithm1 PUBLIC ${CMAKE_SOURCE_DIR})



add_library(algorithm2 SHARED
                algorithm2/Algorithm2.cpp algo_common/BaseAlgorithm.cpp algo_common/AlgorithmGraph.cpp)

target_include_directories(algorithm2 PUBLIC ${CMAKE_SOURCE_DIR})



add_executable(main 
               simulator/main.cpp simulator/AlgorithmRegistrar.cpp simulator/MySimulator.cpp simulator/MySensors.cpp simulator/DevTools.cpp)

target_link_options(main PUBLIC "-rdynamic")
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(main dl)