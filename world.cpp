#include "world.h"

std::vector<bool> blocks;
//note: entities does not include player, since player needs special update
std::vector<Entity> entities;
Entity player;
bool blocksChanged;

void initWorld()
{
  blocks = vector<bool>(WORLD_SIZE * WORLD_SIZE * WORLD_SIZE, false);
  //buildCastle();
  buildPyramid();
  blocksChanged = true;
  player.pos = vec3(-CUBE, 5, -CUBE);
  player.vel = vec3(0, 0, 0);
  player.hitWidth = CUBE;
  player.hitHeight = 2 * CUBE;
  player.hAngle = 0;
  player.vAngle = 0;
}

void renderWorld()
{
  if(!blocksChanged)
    return;
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
  blocksChanged = false;
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

void buildPyramid()
{
  for(int x = 0; x < WORLD_SIZE; x++)
  {
    for(int z = 0; z < WORLD_SIZE; z++)
    {
      int h = WORLD_SIZE / 2 - std::max(abs(x - WORLD_SIZE / 2), abs(z - WORLD_SIZE / 2));
      for(int y = 0; y < h; y++)
      {
        setBlock(x, y, z);
      }
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

void updatePlayer(int xrel, int yrel, bool jumped)
{
  float& hAngle = player.hAngle;
  float& vAngle = player.vAngle;
  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  float speed = SPEED / 60 * CUBE;
  float aheadDist;
  float leftDist;
  aheadDist = speed * ((keystate[SDL_SCANCODE_W] ? 1 : 0) + (keystate[SDL_SCANCODE_S] ? -1 : 0));
  leftDist = speed * ((keystate[SDL_SCANCODE_A] ? 1 : 0) + (keystate[SDL_SCANCODE_D] ? -1 : 0));
  bool onGround = entityOnGround(player);
  float savedVelY = player.vel.y;
  //use hAngle angle to determine displacement from forward-back and left-right movement
  player.vel = aheadDist * vec3(cosf(hAngle), 0, sinf(hAngle));
  player.vel += leftDist * vec3(sinf(hAngle), 0, -cosf(hAngle));
  player.vel.y = savedVelY;
  //update vAngle and hAngle
  hAngle += SENSITIVITY * xrel;
  if(hAngle < 0)
    hAngle += 2 * M_PI;
  else if(hAngle > 2 * M_PI)
    hAngle -= 2 * M_PI;
  vAngle -= SENSITIVITY * yrel;
  //need to cap vAngle to prevent total gimbal lock
  const float maxPitch = 0.98 * M_PI / 2;
  if(vAngle > maxPitch)
    vAngle = maxPitch;
  if(vAngle < -maxPitch)
    vAngle = -maxPitch;
  if(!entityOnGround(player))
  {
    //player in freefall, increase downward velocity
    player.vel.y -= (GRAV / 60.0 * CUBE * CUBE);
  }
  else if(jumped)
  {
    //give the player enouugh upward velocity to reach height JUMP (blocks)
    player.vel.y = JUMP_VEL * CUBE;
  }
  if(!onGround && player.vel.y < 0)
  {
    //player is in freefall and moving downwards
    //if player is falling very fast, need to check collision on all blocks between current and future position
    float newY = player.pos.y + player.vel.y;
    int bx = floorf(player.pos.x / CUBE);
    int bz = floorf(player.pos.z / CUBE);
    int highBlock = floorf(player.pos.y / CUBE);
    int lowBlock = floorf(newY / CUBE);
    for(int i = lowBlock; i < highBlock; i++)
    {
      if(isBlock(bx, i, bz))
      {
        //player hits this block and stops
        player.pos.y = i * CUBE;
        player.vel.y = 0;
        break;
      }
    }
    player.pos += player.vel;
  }
  else
  {
    //on ground, this isn't an issue
    player.pos += player.vel;
  }
  //can't go through the ground
  if(player.pos.y < 0)
  {
    player.pos.y = 0;
    player.vel.y = 0;
  }
}

void updateEntities()
{
  //for(auto& e : entities)
  {
    //position += velocity
    //collision check with blocks, may need to change velocity and/or position
    //TODO: collision check with other entities
  }
}

bool entityOnGround(Entity& e)
{
  //2 necessary conditions for standing on ground:
  //  -pos.y is (almost) exactly on a block boundary
  //  -the block below is solid
  int x, y, z;
  worldToBlock(e.pos, x, y, z);
  float ey = y * CUBE;
  //standing on world bottom
  if(e.pos.y < 1e-4)
    return true;
  if(fabsf(ey - e.pos.y) > 1e-4 || !isBlock(x, y - 1, z))
    return false;
  return true;
}

