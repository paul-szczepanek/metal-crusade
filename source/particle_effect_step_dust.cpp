#include "particle_effect_step_dust.h"
#include "game.h"
#include "arena.h"

ParticleEffectStepDust::ParticleEffectStepDust(Ogre::SceneNode* particle_node)
{
    scene_node = particle_node;

    dust = Game::scene->createParticleSystem(60, "particles");
    dust->setSortingEnabled(true);
    dust->setMaterialName("dust");

    dust->setParameter("particle_width", "10");
    dust->setParameter("particle_height", "10");

    Ogre::ParticleAffector* dust_mover = dust->addAffector("LinearForce");

    dust_mover->setParameter("force_application", "add");
    dust_mover->setParameter("force_vector", "0 -2.5 0");

    //doesn't work! TODO: find out why
    Ogre::ParticleAffector* dust_fader = dust->addAffector("ColourFader");
    dust_fader->setParameter("alpha", "-0.01");

    Ogre::ParticleAffector* dust_scaler = dust->addAffector("Scaler");
    dust_scaler->setParameter("rate", "14");

    Ogre::ParticleAffector* dust_rotator = dust->addAffector("Rotator");
    dust_rotator->setParameter("rotation_speed_range_start", "0");
    dust_rotator->setParameter("rotation_speed_range_end", "20");
    dust_rotator->setParameter("rotation_range_start", "0");
    dust_rotator->setParameter("rotation_range_end", "360");

    dust_emitter = dust->addEmitter("Box");

    dust_emitter->setParameter("width", "4");
    dust_emitter->setParameter("height", "0");
    dust_emitter->setParameter("depth", "2");

    dust_emitter->setAngle(Ogre::Radian(pi));
    dust_emitter->setEmissionRate(0);
    dust_emitter->setMinParticleVelocity(0);
    dust_emitter->setMaxParticleVelocity(1);
    dust_emitter->setTimeToLive(6);

    scene_node->attachObject(dust);

    dust->setEmitting(true);
}

ParticleEffectStepDust::~ParticleEffectStepDust()
{
    Game::scene->destroyParticleSystem(dust);
    die();
}

int ParticleEffectStepDust::update(Ogre::Real a_dt)
{
    return 0;
}

void ParticleEffectStepDust::setRate(Ogre::Real a_rate)
{
    dust_emitter->setEmissionRate(a_rate);
}
