cmake_minimum_required(VERSION 3.22)
project(DynamicSetSampling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
add_executable(DynamicWeightedSetSampling main.cpp ScapegoatTree.hpp BSTSampling.hpp QuickBucket.hpp basic_bst.hpp basic_alias.hpp ../main.cpp)
