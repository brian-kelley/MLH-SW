#include "camera.h"

//speed of player movement, in blocks per second
static const float speed = SPEED / 60 * CUBE;
static vec3 camPos;
static float hAngle;   //player's horizontal view direction (0 = towards +x)
static float vAngle;   //player's vertical view direction (0 = level)

void camInit()
{
  //start in center of world
  camPos = vec3(WORLD_SIZE * CUBE / 2, 1, WORLD_SIZE * CUBE / 2);
  hAngle = 1.5;
  vAngle = 0;
}

void cameraUpdate(int xrel, int yrel)
{
  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  float aheadDist = speed * ((keystate[SDL_SCANCODE_W] ? 1 : 0) + (keystate[SDL_SCANCODE_S] ? -1 : 0));
  float leftDist = speed * ((keystate[SDL_SCANCODE_A] ? 1 : 0) + (keystate[SDL_SCANCODE_D] ? -1 : 0));
  //use hAngle angle to determine displacement from forward-back and left-right movement
  camPos += aheadDist * vec3(cosf(hAngle), 0, sinf(hAngle));
  camPos += leftDist * vec3(sinf(hAngle), 0, -cosf(hAngle));
  //update vAngle and hAngle
  hAngle += SENSITIVITY * xrel;
  if(hAngle < 0)
    hAngle += 2 * M_PI;
  else if(hAngle > 2 * M_PI)
    hAngle -= 2 * M_PI;
  vAngle -= SENSITIVITY * yrel;
  //need to cap vAngle to prevent total gimbal lock
  const float maxPitch = 0.98 * M_PI / 2;
  if(vAngle > maxPitch)
    vAngle = maxPitch;
  if(vAngle < -maxPitch)
    vAngle = -maxPitch;
  //up is always this (no roll allowed)
  vec3 up(0, 1, 0);
  //get a point directly in front of eye (for lookAt) using spherical coordinates
  vec3 center;
  center.x = camPos.x + cosf(vAngle) * cosf(hAngle);
  center.y = camPos.y + sinf(vAngle);
  center.z = camPos.z + cosf(vAngle) * sinf(hAngle);
  mat4 view = lookAt<float>(camPos, center, up);
  mat4 proj = perspective<float>(45.0f, (float) WINW / WINH, 0.1, 100);
  updateMatrices(view, proj);
}

