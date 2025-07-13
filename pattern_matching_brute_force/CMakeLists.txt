cmake_minimum_required(VERSION 3.30)
project(pattern_matching_bruteforce)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_executable(pattern_matching_bruteforce pattern_matching_brute_force.cpp)
