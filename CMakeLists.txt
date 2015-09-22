cmake_minimum_required(VERSION 2.8)
project(CHIP8_emu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

set(SOURCE_FILES main.cpp GameScreen.cpp Emulator.cpp)
add_executable(CHIP8_emu ${SOURCE_FILES})

find_package(PkgConfig REQUIRED)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})


target_link_libraries(CHIP8_emu ${SDL2_LIBRARIES})