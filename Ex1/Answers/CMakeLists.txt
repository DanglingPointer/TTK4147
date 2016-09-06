cmake_minimum_required(VERSION 3.6)
project(AssB)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES ListTest.cpp LinkedList.h)
add_executable(AssB ${SOURCE_FILES})