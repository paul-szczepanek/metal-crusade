#include "arena_entity.h"
#include "game.h"

ArenaEntity::ArenaEntity()
{
}

ArenaEntity::ArenaEntity(const string& a_name)
  : Name(a_name)
{
}

ArenaEntity::~ArenaEntity()
{
}

bool ArenaEntity::handleCollision(Collision* a_collision)
{
  return true;
}
