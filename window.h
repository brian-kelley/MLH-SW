#ifndef WINDOW_H
#define WINDOW_H

#include "general.h"

struct Vertex2D
{
  Vertex2D() : pos(0, 0, 0), color(0, 0, 0, 1) {}
  Vertex2D(vec3 pos, vec4 color)
  {
    this->pos = pos;
    this->color = color;
  }
  vec3 pos;
  vec4 color;
};

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
void beginFrame();
void endFrame();
void updateMatrices(mat4& view, mat4& proj);
void drawTopFace(int x, int y, int z, vec4 color);
void drawBottomFace(int x, int y, int z, vec4 color);
void drawLeftFace(int x, int y, int z, vec4 color);
void drawRightFace(int x, int y, int z, vec4 color);
void drawFrontFace(int x, int y, int z, vec4 color);
void drawBackFace(int x, int y, int z, vec4 color);
void drawCube(int x, int y, int z, vec4 color);
void drawWireCube(int x, int y, int z);
void drawCuboid(vec3 base, float width, float height, vec4 color);
//Draw large square for ground (blocks = world diameter)
void drawGround(int blocks);
//send vertices to VBO
void updateVBO();
void drawHUD();

#endif

