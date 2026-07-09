cmake_minimum_required(VERSION 3.21)
project(elfpeek VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(elfpeek
    src/main.cpp
    src/parser.cpp
)
