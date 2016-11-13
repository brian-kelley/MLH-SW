#include "camera.h"

void cameraUpdate()
{
  vec3 camPos = player.getEyePos();
  vec3 center = camPos + player.getLookDir();
  //up is always this (no roll allowed)
  vec3 up(0, 1, 0);
  //get a point directly in front of eye (for lookAt) using spherical coordinates
  mat4 view = lookAt<float>(camPos, center, up);
  mat4 proj = perspective<float>(45.0f, (float) WINW / WINH, 0.1, 100);
  updateMatrices(view, proj);
}

