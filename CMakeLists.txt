cmake_minimum_required(VERSION 2.8)
project(gameboy)

# Use C++11, enable all warnings and compile in debug mode
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Add any definitions here

# Find packages

# Find include and library paths here

# Global include directories

# gameboy target
# Find all source files in the below directory automatically
file(GLOB_RECURSE SOURCE_FILES
     Cartridge/*.cpp
     CPU/*.cpp
     General/*.cpp
     GPU/*.cpp
     Input/*.cpp
     Memory/*.cpp
     Timer/*.cpp
)

add_executable(gameboy main.cpp ${SOURCE_FILES})
target_include_directories(gameboy PUBLIC .)
