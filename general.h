#ifndef GENERAL_H
#define GENERAL_H

//Global configuration
#define WINW 1280
#define WINH 720

//Note in function declaration for output parameter
#define OUT

#define WORLD_SIZE 80

//Gravitational acceleration: block/s^2
#define GRAV 3

//Player initial upward jump velocity, in blocks/s
#define JUMP_VEL 0.8

//Speed of player (blocks per second)
#define SPEED 6.0

//Mouse sensitivity
#define SENSITIVITY 0.005

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

