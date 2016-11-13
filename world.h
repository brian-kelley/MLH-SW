#ifndef WORLD_H
#define WORLD_H

#include "general.h"
#include "window.h"

void initWorld();
void renderWorld();
void buildCastle();
vec3 blockToWorld(int x, int y, int z);
void worldToBlock(vec3 pos, OUT int& x, OUT int& y, OUT int& z);
bool isBlock(int x, int y, int k);
void setBlock(int x, int y, int k, bool fill = true);

#endif

