cmake_minimum_required(VERSION 2.8)
project(gameboy)

# Use C++11, enable all warnings and compile in debug mode
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Add any definitions here
#set(SDL2_INCLUDE_DIRS "C:/Users/kvnna/ClionProjects/sdl2/include/SDL2")
#set(SDL2_LIBRARIES "-LC:/Users/kvnna/ClionProjects/sdl2/lib -lmingw32 -lSDL2main -lSDL2")

# Find packages
find_package(SDL2 REQUIRED)

# Find include and library paths here

# Global include directories

# gameboy target
# Find all source files in the below directory automatically
file(GLOB_RECURSE SOURCE_FILES
     Cartridge/*.cpp
     CPU/*.cpp
     Engine/*.cpp
     General/*.cpp
     GPU/*.cpp
     Input/*.cpp
     Memory/*.cpp
     Timer/*.cpp
)

add_executable(gameboy main.cpp ${SOURCE_FILES})
target_include_directories(gameboy PUBLIC . ${SDL2_INCLUDE_DIRS})
target_link_libraries(gameboy ${SDL2_LIBRARIES})
