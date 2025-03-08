cmake_minimum_required(VERSION 3.30)
project(pattern_matching)

set(CMAKE_CXX_STANDARD 20)

add_executable(pattern_matching main.cpp
        naive.h
        common.h)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(pattern_matching PRIVATE nlohmann_json::nlohmann_json)
