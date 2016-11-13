#include "window.h"

SDL_Window* window;
SDL_GLContext glContext;
GLuint vshade;
GLuint fshade;
GLuint progID;
GLuint vbo;
GLuint vao;

//Space allocated for vbo (in vertices)
int vboCapacity;

//Shader attributes
GLint posAttribLoc;
GLint colorAttribLoc;
//Shader uniforms
GLint viewLoc;
GLint projLoc;

vec3 camPos;
vector<Vertex> vertices;

void initWindow()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  window = SDL_CreateWindow("Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINW, WINH, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  SDL_ShowCursor(SDL_DISABLE);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  glContext = SDL_GL_CreateContext(window);
  glViewport(0, 0, WINW, WINH);
  glEnable(GL_DEPTH_TEST);
  glClearColor(1, 1, 1, 1);
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
  err[0] = 0;
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
  colorAttribLoc = glGetAttribLocation(progID, "color");
  viewLoc = glGetUniformLocation(progID, "view");
  projLoc = glGetUniformLocation(progID, "proj");
  glDeleteShader(vshade);
  glDeleteShader(fshade);
}

void initVBO()
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  //initialize VBO while VAO is bound
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  vboCapacity = 0;
  glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
  glVertexAttribPointer(colorAttribLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, color));
  glEnableVertexAttribArray(posAttribLoc);
  glEnableVertexAttribArray(colorAttribLoc);
  //unbind VAO
  glBindVertexArray(0);
}

void doFrame()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(progID);
  drawCube(0, 0, 0, vec4(0, 0, 0, 1));
  drawCube(0, 0, 1, vec4(1, 0, 0, 1));
  drawCube(0, 0, 2, vec4(0, 1, 0, 1));
  drawCube(0, 1, 2, vec4(0, 0, 1, 1));
  updateVBO();
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glBindVertexArray(0);
  SDL_GL_SwapWindow(window);
}

void updateMatrices(mat4& view, mat4& proj)
{
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj));
}

void drawCube(int x, int y, int z, vec4 color)
{
  float ax = x * CUBE;
  float ay = y * CUBE;
  float az = z * CUBE;
  float bx = ax + CUBE;
  float by = ay + CUBE;
  float bz = az + CUBE;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  //add the 36 vertices needed for 12 triangles (6 quads)
  vertices.reserve(vertices.size() + 36);
  #define ADD_TRI(a, b, c) \
  { \
    Vertex v1(a, color); \
    Vertex v2(b, color); \
    Vertex v3(c, color); \
    vertices.push_back(v1); \
    vertices.push_back(v2); \
    vertices.push_back(v3); \
  } 
  //front
  ADD_TRI(p000, p100, p110);
  ADD_TRI(p000, p110, p010);
  //top
  ADD_TRI(p000, p100, p101);
  ADD_TRI(p000, p101, p001);
  //left
  ADD_TRI(p000, p001, p010);
  ADD_TRI(p010, p001, p011);
  //right
  ADD_TRI(p100, p101, p111);
  ADD_TRI(p100, p111, p110);
  //back
  ADD_TRI(p101, p001, p111);
  ADD_TRI(p001, p011, p111);
  //bottom
  ADD_TRI(p010, p110, p111);
  ADD_TRI(p010, p111, p011);
}

void updateVBO()
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  if(vertices.size() <= vboCapacity)
  {
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
  }
  else
  {
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);
    vboCapacity = vertices.size();
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

