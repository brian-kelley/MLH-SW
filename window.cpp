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
GLint normalAttribLoc;
//Shader uniforms
GLint viewLoc;
GLint projLoc;
//Lighting uniforms
GLint lightLoc; //uniform light position in both shaders
vec3 lightPos;

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
  glClearColor(0.5, 0.5, 1, 1);
  lightPos = vec3(-2, 40, 5);
  initShaders();
  initVBO();
}

void initShaders()
{
  const char* v =
    "#version 120\n"
    "attribute vec3 position;\n"
    "attribute vec3 normal;\n"
    "attribute vec4 color;\n"
    "varying vec3 fragPos;\n"
    "varying vec4 fragColor;\n"
    "varying vec3 fragNormal;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "uniform vec3 lightPos;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = proj * view * vec4(position.x, position.y, position.z, 1.0);\n"
    "  fragPos = position;\n"
    "  fragNormal = normal;\n"
    "  fragColor = color;\n"
    "}\n";
  const char* f =
    "#version 120\n"
    "varying vec3 fragPos;\n"
    "varying vec3 fragNormal;\n"
    "varying vec4 fragColor;\n"
    "uniform vec3 lightPos;\n"
    "void main()\n"
    "{\n"
    "  vec3 lightDir = fragPos - lightPos;\n"
    "  float dotProd = dot(normalize(lightDir), fragNormal);\n"
    "  dotProd = clamp(dotProd, 0, 1);\n"
    "  float ambient = 0.5;\n"  //max light adds up to 1.0 total (original color)
    "  float diffuse = 0.5 * dotProd;\n"
    "  gl_FragColor = fragColor * (ambient + diffuse);\n"
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
  normalAttribLoc = glGetAttribLocation(progID, "normal");
  viewLoc = glGetUniformLocation(progID, "view");
  projLoc = glGetUniformLocation(progID, "proj");
  lightLoc = glGetUniformLocation(progID, "lightPos");
  glUniform3fv(lightLoc, 3, value_ptr<float>(lightPos));
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
  glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
  glEnableVertexAttribArray(posAttribLoc);
  glEnableVertexAttribArray(colorAttribLoc);
  glEnableVertexAttribArray(normalAttribLoc);
  //unbind VAO
  glBindVertexArray(0);
}

void doFrame()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(progID);
  /*
  drawGround(20);
  drawCube(0, 0, 0, vec4(0.2, 0.2, 0.2, 1));
  drawCube(0, 0, 1, vec4(1, 0, 0, 1));
  drawCube(0, 0, 2, vec4(0, 1, 0, 1));
  drawCube(0, 1, 2, vec4(0, 0, 1, 1));
  */
  updateVBO();
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glBindVertexArray(0);
  SDL_GL_SwapWindow(window);
  vertices.clear();
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
  #define ADD_TRI(a, b, c, norm) \
  { \
    Vertex v1(a, norm, color); \
    Vertex v2(b, norm, color); \
    Vertex v3(c, norm, color); \
    vertices.push_back(v1); \
    vertices.push_back(v2); \
    vertices.push_back(v3); \
  } 
  //front
  ADD_TRI(p000, p100, p110, vec3(0, 0, -1));
  ADD_TRI(p000, p110, p010, vec3(0, 0, -1));
  //top
  ADD_TRI(p000, p100, p101, vec3(0, -1, 0));
  ADD_TRI(p000, p101, p001, vec3(0, -1, 0));
  //left
  ADD_TRI(p000, p001, p010, vec3(-1, 0, 0));
  ADD_TRI(p010, p001, p011, vec3(-1, 0, 0));
  //right
  ADD_TRI(p100, p101, p111, vec3(1, 0, 0));
  ADD_TRI(p100, p111, p110, vec3(1, 0, 0));
  //back
  ADD_TRI(p101, p001, p111, vec3(0, 0, 1));
  ADD_TRI(p001, p011, p111, vec3(0, 0, 1));
  //bottom
  ADD_TRI(p010, p110, p111, vec3(0, 1, 0));
  ADD_TRI(p010, p111, p011, vec3(0, 1, 0));
}

void drawGround(int blocks)
{
  int radius = blocks / 2;
  vec4 color(0, 0.8, 0.1, 1.0f);
  vec3 p1(-radius, 0, -radius);
  vec3 p2(radius, 0, -radius);
  vec3 p3(-radius, 0, radius);
  vec3 p4(radius, 0, radius);
  vec3 upNorm(0, 1, 0);
  vertices.push_back(Vertex(p1, upNorm, color));
  vertices.push_back(Vertex(p2, upNorm, color));
  vertices.push_back(Vertex(p3, upNorm, color));
  vertices.push_back(Vertex(p2, upNorm, color));
  vertices.push_back(Vertex(p3, upNorm, color));
  vertices.push_back(Vertex(p4, upNorm, color));
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

