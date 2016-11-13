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
  player.pos = vec3(-1, 5, -1);
  player.vel = vec3(0, 0, 0);
  player.hitWidth = 4;
  player.hitHeight = 2;
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

void updatePlayer(int xrel, int yrel)
{
  float& hAngle = player.hAngle;
  float& vAngle = player.vAngle;
  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  float speed = SPEED / 60;
  float aheadDist;
  float leftDist;
  aheadDist = speed * ((keystate[SDL_SCANCODE_W] ? 1 : 0) + (keystate[SDL_SCANCODE_S] ? -1 : 0));
  leftDist = speed * ((keystate[SDL_SCANCODE_A] ? 1 : 0) + (keystate[SDL_SCANCODE_D] ? -1 : 0));
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
  updateEntity(player);
}

void updateEntities()
{
  for(auto& e : entities)
  {
    //position += velocity
    //collision check with blocks, may need to change velocity and/or position
    //TODO: collision check with other entities
    if(e.ai)
    {
      //call ai func with self as argument
      e.ai(&e);
    }
    //check for vertical collision
  }
}

//check if any blocks fully or partially occupy the cylindrical disk 1 block high, r meters in radius (x, y also in meters)
static bool checkBlockDisk(float x, float z, float r, int level, OUT int xhit, OUT int zhit)
{
  int bx = floorf(x);
  int bz = floorf(z);
  //most likely possibility: there is a block immediately under the entity
  if(isBlock(bx, level, bz))
  {
    xhit = bx;
    zhit = bz;
    return true;
  }
  //save the square roots inside inner loop
  float rSquared = r * r;
  int blockR = ceil(r);
  for(int i = bx - blockR; i <= bx + blockR; i++)
  {
    for(int j = bz - blockR; j <= bz + blockR; j++)
    {
      //check if block corner is within disk
      float distSquared = (x - i) * (x - i) + (z - j) * (z - j);
      if(distSquared < rSquared)
      {
        //point inside disk, test for solid blocks among the 4 around this point
        if(isBlock(i - 1, level, j - 1))
        {
          xhit = i - 1;
          zhit = j - 1;
          return true;
        }
        if(isBlock(i, level, j - 1))
        {
          xhit = i;
          zhit = j - 1;
          return true;
        }
        if(isBlock(i - 1, level, j))
        {
          xhit = i - 1;
          zhit = j;
          return true;
        }
        if(isBlock(i, level, j))
        {
          xhit = i;
          zhit = j;
          return true;
        }
      }
    }
  }
  return false;
}

void updateEntity(Entity& e)
{
  bool onGround = entityOnGround(e);
  if(!onGround)
  {
    //freefall, increase downward velocity
    e.vel.y -= GRAV / 60.0;
  }
  else if(e.jumped)
  {
    //give the entity enouugh upward velocity to reach height JUMP (blocks)
    e.vel.y = JUMP_VEL;
    e.jumped = false;
  }
  if(!onGround && e.vel.y < 0)
  {
    //freefall and moving downwards
    //if falling very fast, need to check collision on all blocks between current and future position
    float newY = e.pos.y + e.vel.y;
    int bx = floorf(e.pos.x);
    int bz = floorf(e.pos.z);
    int highBlock = floorf(e.pos.y);
    int lowBlock = floorf(newY);
    for(int i = lowBlock; i < highBlock; i++)
    {
      if(isBlock(bx, i, bz))
      {
        //player hits this block and stops
        e.pos.y = i;
        e.vel.y = 0;
        break;
      }
    }
    e.pos += e.vel;
  }
  else
  {
    //on ground, this isn't an issue
    e.pos += e.vel;
  }
  //can't go through the ground
  if(e.pos.y <= 0)
  {
    e.pos.y = 0;
    e.vel.y = 0;
  }
  //test horizontal collision with blocks
}

bool entityOnGround(Entity& e)
{
  //2 necessary conditions for standing on ground:
  //  -pos.y is (almost) exactly on a block boundary
  //  -the block below is solid
  //level is the y of the block that the entity could be standing on
  int level = floorf(e.pos.y);
  float ey = level;
  //standing on world bottom
  if(e.pos.y < 1e-4)
    return true;
  int unused1 = 0;
  int unused2 = 0;
  //need to test all blocks with corners within circular entity base
  if(fabsf(ey - e.pos.y) > 1e-4 || !checkBlockDisk(e.pos.x, e.pos.z, e.hitWidth / 2, level + 1, unused1, unused2))
    return false;
  return true;
}

