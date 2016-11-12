#include "window.h"
#include "camera.h"

int main()
{
  initWindow();
  while(true)
  {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
      if(e.type == SDL_QUIT)
        return 0;
    }
    cameraUpdate();
    doFrame();
  }
  return 0;
}

