cmake_minimum_required(VERSION 3.30)
project(test_case_generator)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_executable(test_case_generator test_case_generator.cpp)
