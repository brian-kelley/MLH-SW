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
  Entity monster;
  monster.pos = vec3(10, 3, -1); 
  monster.vel = vec3(0, 0, 0);
  monster.ai = chaseAI;
  monster.speed = 4 / 60.0;
  monster.hitWidth = 1;
  monster.hitHeight = 2;
  monster.jumpSpeed = 2;
  entities.push_back(monster);
}

void renderWorld()
{
  if(blocksChanged)
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
    blocksChanged = false;
  }
  for(auto& e : entities)
  {
    //draw monsters dark blue
    drawCuboid(e.pos, e.hitWidth, e.hitHeight, vec4(0, 0, 0.3, 1));
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
    updateEntity(e);
  }
}

static bool collision(Entity& e, int* OUT xhit = NULL, int* OUT zhit = NULL)
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
      for(int y = e.pos.y; y < e.pos.y + e.hitHeight; y++)
      {
        if(isBlock(x, y, z))
        {
          if(xhit)
          {
            *xhit = x;
            *zhit = z;
          }
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
  if(e.vel.y > 0)
    return false;
  int r = e.hitWidth / 2;
  int xlo = e.pos.x - r;
  int xhi = e.pos.x + r;
  int zlo = e.pos.z - r;
  int zhi = e.pos.z + r;
  for(int i = xlo; i <= xhi; i++)
  {
    for(int j = zlo; j <= zhi; j++)
    {
      if(isBlock(i, e.pos.y, j))
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
  bool fallOneStep = false;
  if(!onGround)
  {
    //freefall, increase downward velocity
    e.vel.y -= GRAV / 60.0;
    if(floor(e.pos.y) != floor(e.pos.y + e.vel.y))
    {
      float savedY = e.pos.y;
      //test what would happen if entity dropped below the next block
      e.pos.y = ceil(e.pos.y + e.vel.y);
      if(entityOnGround(e))
      {
        //don't fall past ceil(e.pos.y + e.vel.y), would get stuck!
        fallOneStep = true;
      }
      if(!fallOneStep)
      {
        e.pos.y = savedY;
      }
    }
  }
  else if(e.jumped)
  {
    //give the entity enouugh upward velocity to reach height JUMP (blocks)
    e.vel.y = JUMP_VEL;
  }
  e.jumped = false;
  if(fallOneStep)
  {
    e.pos.y = ceil(e.pos.y + e.vel.y);
    e.vel.y = 0;
  }
  else
    e.pos.y += e.vel.y;
  if(collisionY(e))
  {
    e.pos.y = ceil(e.pos.y);
    e.pos.y = e.pos.y < 0 ? 0 : e.pos.y;
    e.vel.y = 0;
  }
  e.pos.x += e.vel.x;
  if(collision(e))
  {
    e.pos.x -= e.vel.x;
    e.vel.x = 0;
  }
  e.pos.z += e.vel.z;
  if(collision(e))
  {
    e.pos.z -= e.vel.z;
    e.vel.z = 0;
  }
}

bool entityOnGround(Entity& e)
{
  if(e.pos.y <= 1e-4)
    return true;
  if(fabsf(e.pos.y - (int) e.pos.y) < 1e-4)
  {
    e.pos.y -= 0.1;
    bool collideBelow = collision(e);
    e.pos.y += 0.1;
    return collideBelow;
  }
  return false;
}

bool getTargetBlock(int& x, int& y, int& z)
{
  vec3 camPos = player.getEyePos();
  vec3 camDir = player.getLookDir();
  vec3 blockIter((int) camPos.x, (int) camPos.y, (int) camPos.z);
  //iterate through blocks, finding the faces that player is looking through
  while(true)
  {
    vec3 nextBlock = blockIter;
    bool haveNext = false;
    if(camDir.x > 0)
    {
      //does camPos + t * camDir pass through the +x face?
      //note: +x face is at blockIter.x + 1
      //just solve for y, z at the point the ray gets to blockIter.x + 1
      vec3 intersect = camPos + ((blockIter.x + 1 - camPos.x) / camDir.x) * camDir;
      if(intersect.y >= blockIter.y && intersect.y < blockIter.y + 1 &&
         intersect.z >= blockIter.z && intersect.z < blockIter.z + 1)
      {
        nextBlock.x += 1;
        haveNext = true;
      }
    }
    if(camDir.x < 0 && !haveNext)
    {
      //-x face
      vec3 intersect = camPos + ((blockIter.x - camPos.x) / camDir.x) * camDir;
      if(intersect.y >= blockIter.y && intersect.y < blockIter.y + 1 &&
         intersect.z >= blockIter.z && intersect.z < blockIter.z + 1)
      {
        nextBlock.x -= 1;
        haveNext = true;
      }
    }
    if(camDir.y > 0 && !haveNext)
    {
      //+y face
      vec3 intersect = camPos + ((blockIter.y + 1 - camPos.y) / camDir.y) * camDir;
      if(intersect.x >= blockIter.x && intersect.x < blockIter.x + 1 &&
         intersect.z >= blockIter.z && intersect.z < blockIter.z + 1)
      {
        nextBlock.y += 1;
        haveNext = true;
      }
    }
    if(camDir.y < 0 && !haveNext)
    {
      //-y face
      vec3 intersect = camPos + ((blockIter.y - camPos.y) / camDir.y) * camDir;
      if(intersect.x >= blockIter.x && intersect.x < blockIter.x + 1 &&
         intersect.z >= blockIter.z && intersect.z < blockIter.z + 1)
      {
        nextBlock.y -= 1;
        haveNext = true;
      }
    }
    if(camDir.z > 0 && !haveNext)
    {
      //+z face
      vec3 intersect = camPos + ((blockIter.z + 1 - camPos.z) / camDir.z) * camDir;
      if(intersect.x >= blockIter.x && intersect.x < blockIter.x + 1 &&
         intersect.y >= blockIter.y && intersect.y < blockIter.y + 1)
      {
        nextBlock.z += 1;
        haveNext = true;
      }
    }
    if(camDir.z < 0 && !haveNext)
    {
      //-z face
      vec3 intersect = camPos + ((blockIter.z - camPos.z) / camDir.z) * camDir;
      if(intersect.x >= blockIter.x && intersect.x < blockIter.x + 1 &&
         intersect.y >= blockIter.y && intersect.y < blockIter.y + 1)
      {
        nextBlock.z -= 1;
        haveNext = true;
      }
    }
    if(!haveNext)
    {
      return false;
    }
    if(isBlock(nextBlock.x, nextBlock.y, nextBlock.z))
    {
      //ray ended in a solid block
      //go through all blocks that player fully or partially occupies, and if any of them match, return false
      //otherwise set output parameters to block coords and return true
      int xlo = floor(player.pos.x - player.hitWidth / 2);
      int xhi = ceil(player.pos.x + player.hitWidth / 2);
      int ylo = floor(player.pos.y);
      int yhi = ceil(player.pos.y + player.hitHeight);
      int zlo = floor(player.pos.z - player.hitWidth / 2);
      int zhi = ceil(player.pos.z + player.hitWidth / 2);
      if(xlo <= blockIter.x && blockIter.x <= xhi &&
         ylo <= blockIter.y && blockIter.y <= yhi &&
         zlo <= blockIter.z && blockIter.z <= zhi)
      {
        return false;
      }
      x = blockIter.x;
      y = blockIter.y;
      z = blockIter.z;
      return true;
    }
    if(((nextBlock.x - camPos.x) * (nextBlock.x - camPos.x) + (nextBlock.y - camPos.y) * (nextBlock.y - camPos.y) + (nextBlock.z - camPos.z) * (nextBlock.z - camPos.z)) > (REACH * REACH))
    {
      //no block in reach
      return false;
    }
    blockIter = nextBlock;
  }
}

void placeBlock(int x, int y, int z)
{
  if(x >= 0 && x < WORLD_SIZE && y >= 0 && y < WORLD_SIZE && z >= 0 && z < WORLD_SIZE)
  {
    blocks[x][y][z] = true;
  }
  blocksChanged = true;
}

