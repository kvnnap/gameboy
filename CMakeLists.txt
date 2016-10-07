cmake_minimum_required(VERSION 2.8)
project(gameboy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

set(SOURCE_FILES main.cpp Cartridge.cpp Cartridge.h)
add_executable(gameboy ${SOURCE_FILES})