#include "window.h"
#include "camera.h"

int main()
{
  initWindow();
  camInit();
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
    }
    cameraUpdate(xrel, yrel);
    doFrame();
    while(SDL_GetTicks() - startTicks < 16);
  }
  return 0;
}

