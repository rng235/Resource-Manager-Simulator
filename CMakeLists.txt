cmake_minimum_required(VERSION 3.3)
project(Lab_3_TEST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp ResourceManager.cpp ResourceManager.h Task.cpp Task.h Verbose.cpp Verbose.h)
add_executable(Lab_3_TEST ${SOURCE_FILES})