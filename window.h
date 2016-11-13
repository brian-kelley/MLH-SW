#ifndef WINDOW_H
#define WINDOW_H

#include "general.h"

struct Vertex
{
  Vertex() : pos(0, 0, 0), normal(0, 1, 0), color(0, 0, 0, 1) {}
  Vertex(vec3 pos, vec3 normal, vec4 color)
  {
    this->pos = pos;
    this->color = color;
    this->normal = normal;
  }
  vec3 pos;
  vec3 normal;
  vec4 color;
};

void initWindow();
void initShaders();
void initVBO();
void initLighting();
void doFrame();
void updateMatrices(mat4& view, mat4& proj);
void drawTopFace(int x, int y, int z, vec4 color);
void drawBottomFace(int x, int y, int z, vec4 color);
void drawLeftFace(int x, int y, int z, vec4 color);
void drawRightFace(int x, int y, int z, vec4 color);
void drawFrontFace(int x, int y, int z, vec4 color);
void drawBackFace(int x, int y, int z, vec4 color);
void drawCube(int x, int y, int z, vec4 color);
//Draw large square for ground (blocks = world diameter)
void drawGround(int blocks);
//send vertices to VBO
void updateVBO();

#endif

