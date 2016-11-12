cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall")

project(Defense)

find_library(SDL2_LIB SDL2)
find_library(GL_LIB OpenGL)

add_executable(Defense
  main.cpp
  window.cpp
)

target_link_libraries(Defense ${SDL2_LIB} ${GL_LIB})

