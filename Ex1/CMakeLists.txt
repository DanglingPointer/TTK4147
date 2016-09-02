cmake_minimum_required(VERSION 3.6)
project(AssB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCE_FILES ListTest.cpp LinkedList.h)
add_executable(AssB ${SOURCE_FILES})