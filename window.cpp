#include "window.h"

SDL_Window* window;
SDL_GLContext glContext;
GLuint vshade;
GLuint fshade;
GLint posAttribLoc;
GLuint progID;
GLuint vbo;
GLuint vao;

void initWindow()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  window = SDL_CreateWindow("Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  glContext = SDL_GL_CreateContext(window);
  glViewport(0, 0, WINW, WINH);
  initShaders();
  initVBO();
}

void initShaders()
{
  const char* v =
    "#version 120\n"
    "attribute vec3 position;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\n";
  const char* f =
    "#version 120\n"
    "void main()\n"
    "{\n"
    "  gl_FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0);\n"
    "}\n";
  vshade = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vshade, 1, &v, NULL);
  glCompileShader(vshade);

  fshade = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fshade, 1, &f, NULL);
  glCompileShader(fshade);
  progID = glCreateProgram();
  glAttachShader(progID, vshade);
  glAttachShader(progID, fshade);
  glLinkProgram(progID);
  glUseProgram(progID);
  posAttribLoc = glGetAttribLocation(progID, "position");
  printf("pos loc: %i\n", posAttribLoc);
  glDeleteShader(vshade);
  glDeleteShader(fshade);
}

void initVBO()
{
  glGenVertexArrays(1, &vao);
  printf("vao id: %i\n", vao);
  glBindVertexArray(vao);
  //initialize VBO while VAO is bound
  glGenBuffers(1, &vbo);
  printf("vbo id: %i\n", vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  float verts[] = {-0.5, 0.5, 0,
                   -0.5, -0.5, 0,
                   0.5, 0.5, 0};
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), verts, GL_STATIC_DRAW);
  glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*) 0);
  glEnableVertexAttribArray(posAttribLoc);
  //unbind VAO
  glBindVertexArray(0);
}

bool doFrame()
{
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0)
  {
    if(e.type == SDL_QUIT)
      return false;
  }
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(progID);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  SDL_GL_SwapWindow(window);
  return true;
}

