#ifndef WORLD_H
#define WORLD_H

#include "general.h"
#include "entity.h"
#include "window.h"

//Initialization
void initWorld();
void buildCastle();
void buildPyramid();

bool isBlock(int x, int y, int k);
void setBlock(int x, int y, int k, bool fill = true);

void renderWorld();

//Update + entity logic
void updateWorld();
void updateEntities();
void updateEntity(Entity& e);
void updatePlayer(int xrel, int yrel);
bool entityOnGround(Entity& e);
extern Entity player;

#endif

