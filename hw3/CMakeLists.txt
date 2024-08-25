cmake_minimum_required(VERSION 3.13)

project(registration_example VERSION 1.0 LANGUAGES CXX)
#set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")


add_library(algorithm_206990236_314978222_a MODULE
algorithm_206990236_314978222_a/Algorithm_206990236_314978222_A.cpp algo_common/BaseAlgorithm.cpp algo_common/AlgorithmGraph.cpp)

target_include_directories(algorithm_206990236_314978222_a PUBLIC ${CMAKE_SOURCE_DIR})



add_library(algorithm_206990236_314978222_b MODULE
algorithm_206990236_314978222_b/Algorithm_206990236_314978222_B.cpp algo_common/BaseAlgorithm.cpp algo_common/AlgorithmGraph.cpp)

target_include_directories(algorithm_206990236_314978222_b PUBLIC ${CMAKE_SOURCE_DIR})



add_executable(myrobot 
               simulator/main.cpp simulator/AlgorithmRegistrar.cpp simulator/MySimulator.cpp simulator/MySensors.cpp simulator/DevTools.cpp)

target_link_options(myrobot PUBLIC "-rdynamic")
target_include_directories(myrobot PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(myrobot dl)