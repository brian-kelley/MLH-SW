#include "world.h"

std::vector<bool> blocks;

void initWorld()
{
  blocks = vector<bool>(WORLD_SIZE * WORLD_SIZE * WORLD_SIZE, false);
  buildCastle();
}

void renderWorld()
{
  drawGround(WORLD_SIZE * 3);
  for(int i = 0; i < WORLD_SIZE; i++)
  {
    for(int j = 0; j < WORLD_SIZE; j++)
    {
      for(int k = 0; k < WORLD_SIZE; k++)
      {
        if(isBlock(i, j, k))
        {
          drawCube(i, j, k, vec4(0.4, 0.4, 0.4, 1.0));
        }
      }
    }
  }
}

void buildCastle()
{
  int height = 4;
  for(int i = 0; i < WORLD_SIZE; i++)
  {
    for(int j = 0; j < height; j++)
    {
      setBlock(i, j, 0);
      setBlock(i, j, WORLD_SIZE - 1);
    }
    if(i % 2 == 0)
    {
      setBlock(i, height, 0);
      setBlock(i, height, WORLD_SIZE - 1);
    }
  }
  for(int i = 0; i < WORLD_SIZE; i++)
  {
    for(int j = 0; j < height; j++)
    {
      setBlock(0, j, i);
      setBlock(WORLD_SIZE - 1, j, i);
    }
    if(i % 2 == 0)
    {
      setBlock(0, height, i);
      setBlock(WORLD_SIZE - 1, height, i);
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
  return blocks[x * WORLD_SIZE * WORLD_SIZE + y * WORLD_SIZE + z];
}

void setBlock(int x, int y, int k, bool fill)
{
  blocks[x * WORLD_SIZE * WORLD_SIZE + y * WORLD_SIZE + k] = fill;
}

