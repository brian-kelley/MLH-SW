#ifndef WORLD_H
#define WORLD_H

#include "general.h"
#include "entity.h"
#include "window.h"

//Initialization
void initWorld();
void buildCastle();
void buildPyramid();

vec3 blockToWorld(int x, int y, int z);
void worldToBlock(vec3 pos, OUT int& x, OUT int& y, OUT int& z);
bool isBlock(int x, int y, int k);
void setBlock(int x, int y, int k, bool fill = true);

void renderWorld();

//Update + entity logic
void updateWorld();
void updateEntities();
void updatePlayer(int xrel, int yrel, bool jumped);
bool entityOnGround(Entity& e);

extern Entity player;

#endif

