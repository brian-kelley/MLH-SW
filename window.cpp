#include "window.h"

SDL_Window* window;
SDL_GLContext glContext;
GLuint vshade;
GLuint fshade;
GLuint progID;
GLuint vbo;
GLuint vao;
bool dynamicUpdate;
bool staticUpdate;
size_t vboDynamicStart;

//Space allocated for vbo (in vertices)
int vboCapacity;

//Shader attributes
GLint posAttribLoc;
GLint colorAttribLoc;
GLint normalAttribLoc;
//Shader uniforms
GLint viewLoc;
GLint projLoc;

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
  initShaders();
  initVBO();
  staticUpdate = false;
  dynamicUpdate = false;
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
    "void main()\n"
    "{\n"
    "  vec3 lightPos;\n"
    "  lightPos.x = 20;\n"
    "  lightPos.y = 50;\n"
    "  lightPos.z = -10;\n"
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
  glDeleteShader(vshade);
  glDeleteShader(fshade);
}

void initVBO()
{
  //Main 3D vao/vbo
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  //initialize VBO while VAO is bound
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  vboCapacity = 0;
  glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, pos));
  glVertexAttribPointer(colorAttribLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, color));
  glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
  glEnableVertexAttribArray(posAttribLoc);
  glEnableVertexAttribArray(colorAttribLoc);
  glEnableVertexAttribArray(normalAttribLoc);
  glBindVertexArray(0);
}

void beginFrame()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(progID);
  updateVBO();
}

void endFrame()
{
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  SDL_GL_SwapWindow(window);
  vertices.erase(vertices.begin() + vboDynamicStart, vertices.end());
}

void updateMatrices(mat4& view, mat4& proj)
{
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj));
}

#define ADD_TRI(a, b, c, norm) \
{ \
  Vertex v1(a, norm, color); \
  Vertex v2(b, norm, color); \
  Vertex v3(c, norm, color); \
  vertices.push_back(v1); \
  vertices.push_back(v2); \
  vertices.push_back(v3); \
} 

void drawTopFace(int x, int y, int z, vec4 color)
{
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  ADD_TRI(p000, p100, p101, vec3(0, -1, 0));
  ADD_TRI(p000, p101, p001, vec3(0, -1, 0));
  staticUpdate = true;
}

void drawBottomFace(int x, int y, int z, vec4 color)
{
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  ADD_TRI(p010, p110, p111, vec3(0, 1, 0));
  ADD_TRI(p010, p111, p011, vec3(0, 1, 0));
  staticUpdate = true;
}

void drawLeftFace(int x, int y, int z, vec4 color)
{
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  ADD_TRI(p000, p001, p010, vec3(-1, 0, 0));
  ADD_TRI(p010, p001, p011, vec3(-1, 0, 0));
  staticUpdate = true;
}

void drawRightFace(int x, int y, int z, vec4 color)
{
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  ADD_TRI(p100, p101, p111, vec3(1, 0, 0));
  ADD_TRI(p100, p111, p110, vec3(1, 0, 0));
  staticUpdate = true;
}

void drawFrontFace(int x, int y, int z, vec4 color)
{
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  ADD_TRI(p000, p100, p110, vec3(0, 0, -1));
  ADD_TRI(p000, p110, p010, vec3(0, 0, -1));
  staticUpdate = true;
}

void drawBackFace(int x, int y, int z, vec4 color)
{
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  ADD_TRI(p101, p001, p111, vec3(0, 0, 1));
  ADD_TRI(p001, p011, p111, vec3(0, 0, 1));
  staticUpdate = true;
}

void drawCube(int x, int y, int z, vec4 color)
{
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  //add the 36 vertices needed for 12 triangles (6 quads)
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
  dynamicUpdate = true;
}

void drawWireCube(int x, int y, int z)
{
  vec4 color(0.7, 0.2, 0.2, 1);
  //add the geometry to the main 3D VBO
  float ax = x;
  float ay = y;
  float az = z;
  float bx = ax + 1;
  float by = ay + 1;
  float bz = az + 1;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
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
  //use last 36 (temporary) vertices in this vbo
  updateVBO();
  //draw with wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(15);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, vertices.size() - 36, 36);
  //restore normal mode
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //get rid of temporary vertices
  for(int i = 0; i < 36; i++)
  {
    vertices.pop_back();
  }
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
  staticUpdate = true;
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

void drawCuboid(vec3 base, float width, float height, vec4 color)
{
  float ax = base.x - width / 2;
  float ay = base.y;
  float az = base.z - width / 2;
  float bx = ax + width;
  float by = ay + height;
  float bz = az + width;
  vec3 p000(ax, ay, az);
  vec3 p001(ax, ay, bz);
  vec3 p010(ax, by, az);
  vec3 p011(ax, by, bz);
  vec3 p100(bx, ay, az);
  vec3 p101(bx, ay, bz);
  vec3 p110(bx, by, az);
  vec3 p111(bx, by, bz);
  dynamicUpdate = true;
  if(vboDynamicStart == -1)
    vboDynamicStart = vertices.size();
  //add the 36 vertices needed for 12 triangles (6 quads)
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

void setDynamicVBO()
{
  vboDynamicStart = vertices.size();
}

void flushGeom()
{
  vertices.clear();
}
