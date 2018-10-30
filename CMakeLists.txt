cmake_minimum_required (VERSION 3.0)
project (stacklang)
add_executable(stacklang interpreter.cpp stacklang.cpp)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
