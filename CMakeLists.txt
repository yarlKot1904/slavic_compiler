cmake_minimum_required(VERSION 3.28)

project(B__)

set(CMAKE_CXX_STANDARD 23)

add_executable(B__ src/main.cpp
        src/Tokenizer.cpp
        src/Tokenizer.hpp
        src/Parser.cpp
        src/Parser.hpp
        src/Generator.cpp
        src/Generator.hpp
        src/arena.hpp
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")