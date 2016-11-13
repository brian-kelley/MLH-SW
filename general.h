#ifndef GENERAL_H
#define GENERAL_H

//Global configuration
#define WINW 1280
#define WINH 720

//Size of cube (in viewspace coordinates)
#define CUBE 0.5

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <OpenGL/gl.h>

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#endif

