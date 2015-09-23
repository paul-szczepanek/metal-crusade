// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEFACTORY_H
#define PARTICLEFACTORY_H

#include "factory.h"

class ParticleEffect;

class ParticleManager : public Factory
{
public:
  ParticleManager();
  virtual ~ParticleManager();

  void update(Ogre::Real a_dt);

  // create explosions
  ParticleEffect* createExplosion(Ogre::SceneNode* scene_node, Ogre::Real a_size,
                                  Ogre::Real a_time, usint a_ferocity = 1);
  ParticleEffect* createExplosion(Ogre::Vector3 a_position, Ogre::Real a_size,
                                  Ogre::Real a_time, usint a_ferocity = 1);
  ParticleEffect* createStepDust(Ogre::SceneNode* scene_node);

private:
  list<ParticleEffect*> effects;
};

#endif // PARTICLEFACTORY_H
