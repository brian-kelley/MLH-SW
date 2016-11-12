#include "camera.h"

float t = 0;
float dt = 0.04;

void cameraUpdate()
{
  t += dt;
  vec3 camPos(5 * cosf(t), 1, 5 * sinf(t));
  vec3 center(0, 0, 0);
  vec3 up(0, 1, 0);
  mat4 view = lookAt<float>(camPos, center, up);
  mat4 proj = perspective<float>(45.0f, (float) WINW / WINH, 0.1, 100);
  updateMatrices(view, proj);
}

