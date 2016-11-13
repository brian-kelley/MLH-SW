#ifndef ENTITY_H
#define ENTITY_H

#include "general.h"

struct Entity
{
  vec3 pos;     //Worldspace position (of hitbox corner)
  vec3 vel;     //Actual velocity
  //Note: for collision simplicity, treating all entity hitboxes as cylinders instead of cuboids
  float hitWidth;   //hitbox width (x and z)
  float hitHeight;  //hitbox height (y)
  float hAngle;
  float vAngle;
  vec3 getLookDir();
  vec3 getEyePos();     //(world space) return 95% up the hitbox height, in center of footprint
};

#endif
