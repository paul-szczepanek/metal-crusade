// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEFACTORY_H
#define PARTICLEFACTORY_H

#include "main.h"

class ParticleEffect;

class ParticleManager
{
public:
  ParticleManager();
  virtual ~ParticleManager();

  void update(Real a_dt);

  // create explosions
  ParticleEffect* createExplosion(Ogre::SceneNode* scene_node,
                                  Real             a_size,
                                  Real             a_time,
                                  usint            a_ferocity = 1);
  ParticleEffect* createExplosion(Vector3 a_position,
                                  Real    a_size,
                                  Real    a_time,
                                  usint   a_ferocity = 1);
  ParticleEffect* createStepDust(Ogre::SceneNode* scene_node);

private:
  list<ParticleEffect*> effects;
};

#endif // PARTICLEFACTORY_H
