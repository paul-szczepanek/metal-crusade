// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#include "main.h"

namespace fx_type
{
enum explosion_type {
  explosion_blast,
  explosion_petrol,
  explosion_smoke,
  explosion_water,
};

};

class ParticleEffect
{
public:
  ParticleEffect();
  virtual ~ParticleEffect();

  virtual bool update(const Real a_dt) = 0;

protected:
  void die();

  Ogre::SceneNode* SceneNode;

  Real CurrentLifetime;
  Real LifetimeLimit;
};

#endif // PARTICLEEFFECT_H
