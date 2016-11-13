#include "window.h"
#include "camera.h"
#include "world.h"

int main()
{
  initWindow();
  initWorld();
  while(true)
  {
    const int startTicks = SDL_GetTicks();
    int xrel = 0;
    int yrel = 0;
    bool jumped = false;
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
        if(e.key.keysym.scancode == SDL_SCANCODE_SPACE)
        {
          jumped = true;
        }
      }
    }
    updatePlayer(xrel, yrel, jumped);
    updateEntities();
    cameraUpdate();
    renderWorld();
    doFrame();
    while(SDL_GetTicks() - startTicks < 16);
  }
  return 0;
}

