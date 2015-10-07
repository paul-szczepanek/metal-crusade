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

bool ArenaEntity::update(const Real a_dt)
{
  return true;
}

// targeting

bool ArenaEntity::acquireAsTargetBy(ArenaEntity* a_entity)
{
  return false;
}

void ArenaEntity::releaseAsTarget(ArenaEntity* a_targeted_by)
{
}

bool ArenaEntity::loseTarget(ArenaEntity* a_targeted_by,
                             bool         a_forced)
{
  return true;
}

ArenaEntity* ArenaEntity::getTarget()
{
  return this;
}

void ArenaEntity::clearFromTargets()
{
}

Corpus* ArenaEntity::getGround()
{
  return NULL;
}

Vector3 ArenaEntity::getDirection()
{
  return Vector3::ZERO;
}

Quaternion ArenaEntity::getOrientation()
{
  return Quaternion::IDENTITY;
}
