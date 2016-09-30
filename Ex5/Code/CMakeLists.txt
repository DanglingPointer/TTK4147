cmake_minimum_required(VERSION 3.6)
project(Code)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -lcomedi -lrt -pthread -lm ")

set(SOURCE_FILES main.cpp io.cpp)
add_executable(Code ${SOURCE_FILES})