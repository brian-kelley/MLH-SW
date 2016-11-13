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


