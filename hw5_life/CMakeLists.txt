cmake_minimum_required(VERSION 3.2)
project(life)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_EXE_LINKER_FLAGS -fopenmp)

add_executable(life life.c)
