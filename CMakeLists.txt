cmake_minimum_required(VERSION 3.28)
project(DataStructure-UnitTest)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options("/utf-8")
endif()

# google test
add_subdirectory("deps/googletest")
enable_testing(TRUE)

# unit tests
add_subdirectory("Deap")
add_subdirectory("MinMaxHeap")
