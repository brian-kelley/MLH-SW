#ifndef WINDOW_H
#define WINDOW_H

#include "general.h"

struct Vertex
{
  Vertex() : pos(0, 0, 0), color(0, 0, 0, 1) {}
  Vertex(vec3 pos, vec4 color)
  {
    this->pos = pos;
    this->color = color;
  }
  vec3 pos;
  vec4 color;
};

void initWindow();
void initShaders();
void initVBO();
void doFrame();
void updateMatrices(mat4& view, mat4& proj);
void drawCube(int x, int y, int z, vec4 color);
//send vertices to VBO
void updateVBO();

#endif

