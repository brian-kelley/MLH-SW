#include "window.h"
#include "camera.h"
#include "world.h"

int main()
{
  initWindow();
  initWorld();
  int targetX = 0;
  int targetY = 0;
  int targetZ = 0;
  bool haveFrameCube = false;
  bool paused = false;
  while(true)
  {
    const int startTicks = SDL_GetTicks();
    int xrel = 0;
    int yrel = 0;
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
      if(e.type == SDL_QUIT)
        return 0;
      if(e.type == SDL_MOUSEMOTION)
      {
        xrel = e.motion.xrel;
        yrel = e.motion.yrel;
      }
      if(e.type == SDL_KEYDOWN)
      {
        if(e.key.keysym.scancode == SDL_SCANCODE_SPACE && e.key.repeat == 0)
        {
          player.jumped = true;
        }
        else if(e.key.keysym.scancode == SDL_SCANCODE_R)
        {
          initWorld();
        }
        else if(e.key.keysym.scancode == SDL_SCANCODE_P)
        {
          paused = !paused;
        }
      }
      if(e.type == SDL_MOUSEBUTTONDOWN)
      {
        if(haveFrameCube && e.button.state == SDL_PRESSED)
        {
          placeBlock(targetX, targetY, targetZ);
        }
      }
    }
    if(!paused)
    {
      //Spawn a monster every ~10 seconds
      if(rand() % 600 == 599)
      {
        spawnMonster();
      }
      updatePlayer(xrel, yrel);
      updateEntities();
      cameraUpdate();
      haveFrameCube = getTargetBlock(targetX, targetY, targetZ);
      renderWorld();
      beginFrame();
      if(haveFrameCube)
      {
        drawWireCube(targetX, targetY, targetZ);
      }
      endFrame();
    }
    while(SDL_GetTicks() - startTicks < 16);
  }
  return 0;
}

