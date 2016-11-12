#include "window.h"

SDL_Window* window;
SDL_GLContext glContext;
GLuint vshade;
GLuint fshade;
GLuint progID;
GLuint vbo;
GLuint vao;

//Shader attributes
GLint posAttribLoc;
GLint colorAttribLoc;
//Shader uniforms
GLint viewLoc;
GLint projLoc;

vec3 camPos;

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
    "attribute vec4 color;\n"
    "varying vec4 fragColor;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = proj * view * vec4(position.x, position.y, position.z, 1.0);\n"
    "  fragColor = color;\n"
    "}\n";
  const char* f =
    "#version 120\n"
    "varying vec4 fragColor;\n"
    "void main()\n"
    "{\n"
    "  gl_FragColor = fragColor;\n"
    "}\n";
  char err[500];
  err[0] = 0;
  vshade = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vshade, 1, &v, NULL);
  glCompileShader(vshade);
  glGetShaderInfoLog(vshade, 500, NULL, err);
  if(err[0])
  {
    printf("vshade error: %s\n", err);
  }
  fshade = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fshade, 1, &f, NULL);
  glCompileShader(fshade);
  glGetShaderInfoLog(fshade, 500, NULL, err);
  if(err[0])
  {
    printf("fshade error: %s\n", err);
  }
  progID = glCreateProgram();
  glAttachShader(progID, vshade);
  glAttachShader(progID, fshade);
  glLinkProgram(progID);
  glUseProgram(progID);
  posAttribLoc = glGetAttribLocation(progID, "position");
  printf("pos loc: %i\n", posAttribLoc);
  colorAttribLoc = glGetAttribLocation(progID, "color");
  printf("color loc: %i\n", colorAttribLoc);
  viewLoc = glGetUniformLocation(progID, "view");
  printf("view loc: %i\n", viewLoc);
  projLoc = glGetUniformLocation(progID, "proj");
  printf("proj loc: %i\n", projLoc);
  glDeleteShader(vshade);
  glDeleteShader(fshade);
}

struct Vertex
{
  vec3 pos;
  vec4 color;
};

void initVBO()
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  //initialize VBO while VAO is bound
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  Vertex verts[3];
  //triangle positions
  verts[0].pos = {-0.5, 0.5, 0};
  verts[1].pos = {-0.5, -0.5, 0};
  verts[2].pos = {0.5, 0.5, 0};
  verts[0].color = {1, 0, 0, 1};
  verts[1].color = {0, 1, 0, 1};
  verts[2].color = {0, 0, 1, 1};
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex), verts, GL_STATIC_DRAW);
  glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
  glVertexAttribPointer(colorAttribLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, color));
  glEnableVertexAttribArray(posAttribLoc);
  glEnableVertexAttribArray(colorAttribLoc);
  //unbind VAO
  glBindVertexArray(0);
}

void doFrame()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(progID);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  SDL_GL_SwapWindow(window);
}

void updateMatrices(mat4& view, mat4& proj)
{
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj));
}

