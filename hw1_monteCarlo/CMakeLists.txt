cmake_minimum_required(VERSION 2.6)
project(monteCarlo)

find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES main.c functions.h test.h)
add_executable(monteCarlo ${SOURCE_FILES})
target_link_libraries(monteCarlo ${CMAKE_THREAD_LIBS_INIT} m)
