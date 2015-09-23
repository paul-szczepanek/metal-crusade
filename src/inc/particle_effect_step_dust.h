// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEEFFECTSTEPDUST_H
#define PARTICLEEFFECTSTEPDUST_H

#include "particle_effect.h"

class ParticleEffectStepDust
  : public ParticleEffect
{
public:
  ParticleEffectStepDust(Ogre::SceneNode* scene_node);
  ~ParticleEffectStepDust();

  int update(Real a_dt);

  void setRate(Real a_rate);

private:
  Ogre::ParticleSystem* dust;
  Ogre::ParticleEmitter* dust_emitter;

};

#endif // PARTICLEEFFECTSTEPDUST_H
