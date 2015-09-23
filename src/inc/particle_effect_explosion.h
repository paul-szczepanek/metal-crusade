// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEEFFECTEXPLOSION_H
#define PARTICLEEFFECTEXPLOSION_H

#include "particle_effect.h"


class ParticleEffectExplosion : public ParticleEffect
{
public:
  ParticleEffectExplosion(Ogre::SceneNode* scene_node, Ogre::Real a_size, Ogre::Real a_time,
                          Ogre::Real a_ferocity);
  ~ParticleEffectExplosion();

  int update(Ogre::Real a_dt);

private:
  Ogre::ParticleSystem* explosion;

};

#endif // PARTICLEEFFECTEXPLOSION_H
