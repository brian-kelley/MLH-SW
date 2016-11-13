#include "world.h"

bool blocks[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];
//note: entities does not include player, since player needs special update
std::vector<Entity> entities;
Entity player;
bool blocksChanged;

void initWorld()
{
  memset(blocks, 0, sizeof(bool) * WORLD_SIZE * WORLD_SIZE * WORLD_SIZE);
  //buildCastle();
  buildPyramid();
  blocksChanged = true;
  player.pos = vec3(-1, 5, -1);
  player.vel = vec3(0, 0, 0);
  player.hitWidth = 1;
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
      blocks[i][j][0] = true;
      blocks[i][j][width] = true;
    }
    if(i % 2 == 0)
    {
      blocks[i][height][0] = true;
      blocks[i][height][width] = true;
    }
  }
  for(int i = 0; i < width + 1; i++)
  {
    for(int j = 0; j < height; j++)
    {
      blocks[0][j][i] = true;
      blocks[width][j][i] = true;
    }
    if(i % 2 == 0)
    {
      blocks[0][height][i] = true;
      blocks[width][height][i] = true;
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
        blocks[x][y][z] = true;
      }
    }
  }
}

bool isBlock(int x, int y, int z)
{
  if(x < 0 || y < 0 || z < 0 || x >= WORLD_SIZE || y >= WORLD_SIZE || z >= WORLD_SIZE)
    return false;
  return blocks[x][y][z];
}

void setBlock(int x, int y, int z, bool fill)
{
  blocks[x][y][z] = fill;
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
  printf("Player pos: %.1f, %.1f, %.1f\n", player.pos.x, player.pos.y, player.pos.z);
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

static bool collision(Entity& e)
{
  float r = e.hitWidth / 2;
  int xlo = floor(e.pos.x - r);
  int xhi = floor(e.pos.x + r);
  int zlo = floor(e.pos.z - r);
  int zhi = floor(e.pos.z + r);
  for(int x = xlo; x <= xhi; x++)
  {
    for(int z = zlo; z <= zhi; z++)
    {
      for(int y = floorf(e.pos.y); y < floorf(e.pos.y) + e.hitHeight; y++)
      {
        if(isBlock(x, y, z))
        {
          return true;
        }
      }
    }
  }
  return false;
}

static bool collisionY(Entity& e)
{
  if(e.pos.y < 0)
    return true;
  int r = e.hitWidth / 2;
  int xlo = floorf(e.pos.x - r);
  int xhi = ceilf(e.pos.x + r);
  int zlo = floorf(e.pos.z - r);
  int zhi = ceilf(e.pos.z + r);
  for(int i = xlo; i <= xhi; i++)
  {
    for(int j = zlo; j <= zhi; j++)
    {
      if(isBlock(i, floorf(e.pos.y), j))
      {
        return true;
      }
    }
  }
  return false;
}

void updateEntity(Entity& e)
{
  bool onGround = entityOnGround(e);
  if(onGround)
    puts("On ground");
  else
    puts("In air");
  if(!onGround)
  {
    //freefall, increase downward velocity
    e.vel.y -= GRAV / 60.0;
  }
  else if(e.jumped)
  {
    //give the entity enouugh upward velocity to reach height JUMP (blocks)
    e.vel.y = JUMP_VEL;
  }
  e.jumped = false;
  e.pos.y += e.vel.y;
  if(collisionY(e))
  {
    puts("Collided in y.");
    e.pos.y = ceil(e.pos.y);
    e.pos.y = e.pos.y < 0 ? 0 : e.pos.y;
    e.vel.y = 0;
  }
  e.pos.x += e.vel.x;
  if(collision(e))
  {
    puts("Collided in x.");
    e.pos.x -= e.vel.x;
    e.vel.x = 0;
  }
  e.pos.z += e.vel.z;
  if(collision(e))
  {
    puts("Collided in z.");
    e.pos.z -= e.vel.z;
    e.vel.z = 0;
  }
}

bool entityOnGround(Entity& e)
{
  printf("e.pos.y is %f\n", e.pos.y);
  printf("floor(e.pos.y) = %f\n", floorf(e.pos.y));
  if(e.pos.y - (int) e.pos.y < 1e-4 && isBlock(e.pos.x, e.pos.y, e.pos.z))
    return true;
  if(e.pos.y <= 1e-4)
    return true;
  return false;
}

