// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEEFFECTSTEPDUST_H
#define PARTICLEEFFECTSTEPDUST_H

#include "particle_effect.h"

class ParticleEffectStepDust
  : public ParticleEffect
{
public:
  ParticleEffectStepDust(Ogre::SceneNode* SceneNode);
  ~ParticleEffectStepDust();

  bool update(const Real a_dt);

  void setRate(Real a_rate);

private:
  Ogre::ParticleSystem* Dust;
  Ogre::ParticleEmitter* DustEmitter;

};

#endif // PARTICLEEFFECTSTEPDUST_H
