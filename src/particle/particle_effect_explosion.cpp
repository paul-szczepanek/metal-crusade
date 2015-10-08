// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "particle_effect_explosion.h"
#include "game.h"

ParticleEffectExplosion::ParticleEffectExplosion(Ogre::SceneNode* particle_node,
                                                 Real             a_size,
                                                 Real             a_time,
                                                 Real             a_ferocity)
{
  SceneNode = particle_node;
  CurrentLifetime = 0;
  LifetimeLimit = a_time + 0.25;

  Explosion = Game::Scene->createParticleSystem(10, "particles");
  Explosion->setSortingEnabled(true);
  Explosion->setMaterialName("explosion");
  Explosion->setParameter("particle_width", "6");
  Explosion->setParameter("particle_height", "1");

  // scale up
  Ogre::ParticleAffector* explosion_scaler = Explosion->addAffector("Scaler");
  explosion_scaler->setParameter("rate", realIntoString(a_size / (a_time * 1.25)));

  // rotate
  Ogre::ParticleAffector* explosion_rotator = Explosion->addAffector("Rotator");
  explosion_rotator->setParameter("rotation_speed_range_start", "-180");
  explosion_rotator->setParameter("rotation_speed_range_end", "180");
  explosion_rotator->setParameter("rotation_range_start", "0");
  explosion_rotator->setParameter("rotation_range_end", "360");

  // emit from a sphere
  Ogre::ParticleEmitter* explosion_emitter = Explosion->addEmitter("Ellipsoid");
  explosion_emitter->setParameter("width", "4");
  explosion_emitter->setParameter("height", "4");
  explosion_emitter->setParameter("depth", "4");
  explosion_emitter->setAngle(Radian(pi));
  explosion_emitter->setEmissionRate(a_ferocity * 2);
  explosion_emitter->setMinParticleVelocity(1);
  explosion_emitter->setMaxParticleVelocity(4);
  explosion_emitter->setTimeToLive(a_time);
  explosion_emitter->setDuration(0.25);

  SceneNode->attachObject(Explosion);

  Explosion->setEmitting(true);
}

ParticleEffectExplosion::~ParticleEffectExplosion()
{
  Game::Scene->destroyParticleSystem(Explosion);
  die();
}

bool ParticleEffectExplosion::update(const Real a_dt)
{
  CurrentLifetime += a_dt;

  // kill after lifetime ends
  if (CurrentLifetime > LifetimeLimit) {
    return false;
  }

  return true;
}
