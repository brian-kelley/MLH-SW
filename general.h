#ifndef GENERAL_H
#define GENERAL_H

//Global configuration
#define WINW 1280
#define WINH 720

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <OpenGL/gl.h>

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

#include <glm/glm.hpp>

#endif

