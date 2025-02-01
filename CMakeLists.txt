cmake_minimum_required(VERSION 3.31.4)

project(lemon)
include_directories(include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(lemon src/lemon.cpp src/battery.cpp src/functions.cpp src/lemonOutput.cpp)
