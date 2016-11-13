#include "world.h"

std::vector<bool> blocks;

void initWorld()
{
  blocks = vector<bool>(WORLD_SIZE * WORLD_SIZE * WORLD_SIZE, false);
  buildCastle();
}

void renderWorld()
{
  drawGround(WORLD_SIZE * 100);
  vec4 stoneColor(0.4, 0.4, 0.4, 1.0);
  for(int i = 0; i < WORLD_SIZE + 1; i++)
  {
    for(int j = 0; j < WORLD_SIZE + 1; j++)
    {
      for(int k = 0; k < WORLD_SIZE + 1; k++)
      {
        //at each block, draw top, front and left face only if the face is exposed
        if(isBlock(i, j, k) ^ isBlock(i, j - 1, k) && j > 0)
        {
          drawTopFace(i, j, k, stoneColor);
        }
        if(isBlock(i, j, k) ^ isBlock(i, j, k - 1))
        {
          drawFrontFace(i, j, k, stoneColor);
        }
        if(isBlock(i, j, k) ^ isBlock(i - 1, j, k))
        {
          drawLeftFace(i, j, k, stoneColor);
        }
      }
    }
  }
}

void buildCastle()
{
  int height = 4;
  int width = 9;//WORLD_SIZE % 2 == 0 ? WORLD_SIZE - 2 : WORLD_SIZE - 1;
  for(int i = 0; i < width + 1; i++)
  {
    for(int j = 0; j < height; j++)
    {
      setBlock(i, j, 0);
      setBlock(i, j, width);
    }
    if(i % 2 == 0)
    {
      setBlock(i, height, 0);
      setBlock(i, height, width);
    }
  }
  for(int i = 0; i < width + 1; i++)
  {
    for(int j = 0; j < height; j++)
    {
      setBlock(0, j, i);
      setBlock(width, j, i);
    }
    if(i % 2 == 0)
    {
      setBlock(0, height, i);
      setBlock(width, height, i);
    }
  }
}

vec3 blockToWorld(int x, int y, int z)
{
  return vec3(x * CUBE, y * CUBE, z * CUBE);
}

void worldToBlock(vec3 pos, OUT int& x, OUT int& y, OUT int& z)
{
  x = floorf(pos.x / CUBE);
  y = floorf(pos.y / CUBE);
  z = floorf(pos.z / CUBE);
}

bool isBlock(int x, int y, int z)
{
  if(x < 0 || y < 0 || z < 0 || x >= WORLD_SIZE || y >= WORLD_SIZE || z >= WORLD_SIZE)
    return false;
  return blocks[x * WORLD_SIZE * WORLD_SIZE + y * WORLD_SIZE + z];
}

void setBlock(int x, int y, int k, bool fill)
{
  blocks[x * WORLD_SIZE * WORLD_SIZE + y * WORLD_SIZE + k] = fill;
}

