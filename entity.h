#ifndef ENTITY_H
#define ENTITY_H

#include "general.h"

//Need access to player here because monsters need to know where player is

typedef void (*aiFunc)(void*);

extern bool isBlock(int x, int y, int z);

struct Entity
{
  vec3 pos;     //Worldspace position (of hitbox corner)
  vec3 vel;     //Actual velocity
  //Note: for collision simplicity, treating all entity hitboxes as cylinders instead of cuboids
  float hitWidth;   //hitbox width (x and z)
  float hitHeight;  //hitbox height (y)
  float hAngle;
  float vAngle;
  float speed;
  bool jumped;          //whether the entity wants to jump on the next timestep
  bool alive;
  float jumpSpeed;
  aiFunc ai;            //function that autonomously changes velocity and performs actions (player's is NULL)
  vec3 getLookDir();
  vec3 getEyePos();     //(world space) return 95% up the hitbox height, in center of footprint
};

void chaseAI(void* self);

extern Entity player;

#endif

