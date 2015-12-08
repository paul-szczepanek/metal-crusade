// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "dynamic_entity.h"
#include "corpus.h"
#include "game.h"
#include "game_arena.h"

DynamicEntity::DynamicEntity()
  : Ground(this)
{
  Ground.loadCollision("ground");
  Ground.CollisionType = collision_type_blocking;
  Ground.Weight = BLOCKING_WEIGHT;
  Ground.OnArena = true;
  Ground.Penetration = 0;
  Ground.SurfaceTemperature = 0;
  Ground.Friction = 0.9;
  Ground.Hardness = 0.2;
  Ground.Conductivity = 0;
  Ground.BallisticDmg = 0;
  Ground.HeatDmg = 0;
  Ground.EnergyDmg = 0;
}

DynamicEntity::~DynamicEntity()
{
}

Corpus* DynamicEntity::getGround()
{
  updateGround();
  return &Ground;
}

void DynamicEntity::updateGround()
{
  // ground is not a real dynamic object so we have to update it manually
  Ground.XYZ.x = getXYZ().x;
  Ground.XYZ.y = Game::Arena->getHeight(getXYZ().x, getXYZ().z) - GROUND_COLLISION_RADIUS;
  Ground.XYZ.z = getXYZ().z;
  // it's not handled by the corpus manager
  Ground.update(0);
}
