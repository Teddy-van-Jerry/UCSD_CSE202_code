cmake_minimum_required(VERSION 3.10)
project(hw2_dijkstra_priority_queue VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")
add_executable(hw2_dijkstra_priority_queue ${SOURCES})

enable_testing()
add_test(NAME t       COMMAND hw2_dijkstra_priority_queue)
add_test(NAME n10_m10 COMMAND hw2_dijkstra_priority_queue 10 100)
