#include "entity.h"

vec3 Entity::getLookDir()
{
  vec3 dir;
  dir.x = cosf(vAngle) * cosf(hAngle);
  dir.y = sinf(vAngle);
  dir.z = cosf(vAngle) * sinf(hAngle);
  return dir;
}

vec3 Entity::getEyePos()
{
  //return 95% up the hitbox height, in center of footprint 
  vec3 rv = pos;
  rv.y += 0.95 * hitHeight;
  return rv;
}

void chaseAI(void* e)
{
  Entity& ent = *((Entity*) e);
  //set velocity = speed, in direction of player
  float xToPlayer = player.pos.x - ent.pos.x;
  float zToPlayer = player.pos.z - ent.pos.z;
  float theta = atan2f(zToPlayer, xToPlayer);
  ent.vel.x = ent.speed * cos(theta);
  ent.vel.z = ent.speed * sin(theta);
  //jump only if it helps get towards the player
  if(fabsf(ent.vel.x) > fabsf(ent.vel.z))
  {
    if(ent.vel.x > 0 && isBlock(ent.pos.x + 1, ent.pos.y, ent.pos.z))
      ent.jumped = true;
    else if(ent.vel.x < 0 && isBlock(ent.pos.x - 1, ent.pos.y, ent.pos.z))
      ent.jumped = true;
  }
  else
  {
    if(ent.vel.z > 0 && isBlock(ent.pos.x, ent.pos.y, ent.pos.z + 1))
      ent.jumped = true;
    else if(ent.vel.z < 0 && isBlock(ent.pos.x, ent.pos.y, ent.pos.z - 1))
      ent.jumped = true;
  }
}

