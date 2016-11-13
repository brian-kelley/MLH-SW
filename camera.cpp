#include "camera.h"

//multiply by mouse motion in pixels to get angle update
static const float sensitivity = 1.0 / 203;
//speed of player movement, in blocks per second
static const float speed = 0.4 * CUBE;
static vec3 camPos;
static float hAngle;   //player's horizontal view direction (0 = towards +x)
static float vAngle;   //player's vertical view direction (0 = level)

void camInit()
{
  camPos = vec3(4, 1, 0);
  hAngle = 0;
  vAngle = 0;
}

void cameraUpdate(int xrel, int yrel)
{
  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  float aheadDist = speed * ((keystate[SDL_SCANCODE_W] ? 1 : 0) + (keystate[SDL_SCANCODE_S] ? -1 : 0));
  float leftDist = speed * ((keystate[SDL_SCANCODE_A] ? 1 : 0) + (keystate[SDL_SCANCODE_D] ? -1 : 0));
  //use hAngle angle to determine displacement from forward-back and left-right movement
  camPos += aheadDist * vec3(cosf(hAngle), 0, sinf(hAngle));
  camPos += leftDist * vec3(sinf(hAngle), 0, cosf(hAngle));
  //update vAngle and hAngle
  hAngle += sensitivity * xrel;
  if(hAngle < 0)
    hAngle += 2 * M_PI;
  else if(hAngle > 2 * M_PI)
    hAngle -= 2 * M_PI;
  vAngle -= sensitivity * yrel;
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
  printf("Camera @ (%.2f, %.2f, %.2f)\n", camPos.x, camPos.y, camPos.z);
}

