// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "particle_effect.h"
#include "game.h"

ParticleEffect::ParticleEffect()
{
  // ctor
}

ParticleEffect::~ParticleEffect()
{
  // dtor
}

void ParticleEffect::die()
{
  Game::scene->destroySceneNode(scene_node);
}
