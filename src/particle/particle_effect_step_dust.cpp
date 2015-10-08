// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "particle_effect_step_dust.h"
#include "game.h"
#include "game_arena.h"

ParticleEffectStepDust::ParticleEffectStepDust(Ogre::SceneNode* particle_node)
{
  SceneNode = particle_node;

  Dust = Game::Scene->createParticleSystem(60, "particles");
  Dust->setSortingEnabled(true);
  Dust->setMaterialName("dust");
  Dust->setParameter("particle_width", "10");
  Dust->setParameter("particle_height", "10");

  // move the dust under the terrain over time
  Ogre::ParticleAffector* dust_mover = Dust->addAffector("LinearForce");
  dust_mover->setParameter("force_application", "add");
  dust_mover->setParameter("force_vector", "0 -2.5 0");

  // doesn't work! TODO: find out why
  Ogre::ParticleAffector* dust_fader = Dust->addAffector("ColourFader");
  dust_fader->setParameter("alpha", "-0.01");

  // scale up the dust as it settles
  Ogre::ParticleAffector* dust_scaler = Dust->addAffector("Scaler");
  dust_scaler->setParameter("rate", "14");

  // rotate dust
  Ogre::ParticleAffector* dust_rotator = Dust->addAffector("Rotator");
  dust_rotator->setParameter("rotation_speed_range_start", "0");
  dust_rotator->setParameter("rotation_speed_range_end", "20");
  dust_rotator->setParameter("rotation_range_start", "0");
  dust_rotator->setParameter("rotation_range_end", "360");

  // emit in a plane under the feet
  DustEmitter = Dust->addEmitter("Box");

  DustEmitter->setParameter("width", "6");
  DustEmitter->setParameter("height", "0");
  DustEmitter->setParameter("depth", "2");
  DustEmitter->setAngle(Radian(pi));
  DustEmitter->setEmissionRate(0);
  DustEmitter->setMinParticleVelocity(0);
  DustEmitter->setMaxParticleVelocity(1);
  DustEmitter->setTimeToLive(6);

  SceneNode->attachObject(Dust);

  Dust->setEmitting(true);
}

ParticleEffectStepDust::~ParticleEffectStepDust()
{
  Game::Scene->destroyParticleSystem(Dust);
  die();
}

bool ParticleEffectStepDust::update(const Real a_dt)
{
  return true;
}

void ParticleEffectStepDust::setRate(Real a_rate)
{
  // the faster the crusader goes more dust gets upset
  DustEmitter->setEmissionRate(a_rate * a_rate * 0.025);

  // this is a really cheap way to set the dust size
  if (a_rate > 8) {
    Dust->setParameter("particle_width", "12");
    Dust->setParameter("particle_height", "12");
  } else if (a_rate > 2) {
    Dust->setParameter("particle_width", "6");
    Dust->setParameter("particle_height", "6");
  } else {
    Dust->setParameter("particle_width", "3");
    Dust->setParameter("particle_height", "3");
  }
}
