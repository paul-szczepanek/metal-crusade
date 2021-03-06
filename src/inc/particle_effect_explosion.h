// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEEFFECTEXPLOSION_H
#define PARTICLEEFFECTEXPLOSION_H

#include "particle_effect.h"

class ParticleEffectExplosion
  : public ParticleEffect
{
public:
  ParticleEffectExplosion(Ogre::SceneNode* SceneNode,
                          Real             a_size,
                          Real             a_time,
                          Real             a_ferocity);
  ~ParticleEffectExplosion();

  bool update(const Real a_dt);

private:
  Ogre::ParticleSystem* Explosion;

};

#endif // PARTICLEEFFECTEXPLOSION_H
