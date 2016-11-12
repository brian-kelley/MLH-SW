#include "window.h"

SDL_Window* window;
SDL_GLContext glContext;

void initWindow()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  window = SDL_CreateWindow("Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  glContext = SDL_GL_CreateContext(window);
}

bool doFrame()
{
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0)
  {
    if(e.type == SDL_QUIT)
      return false;
  }
  glClearColor(1.0, 0.5, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
  return true;
}

