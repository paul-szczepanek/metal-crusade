#include "particle_effect.h"
#include "game.h"

ParticleEffect::ParticleEffect()
{
    //ctor
}

ParticleEffect::~ParticleEffect()
{
    //dtor
}

void ParticleEffect::die()
{
    Game::scene->destroySceneNode(scene_node);
}
