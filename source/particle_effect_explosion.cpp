#include "particle_effect_explosion.h"
#include "game.h"

ParticleEffectExplosion::ParticleEffectExplosion(Ogre::SceneNode* particle_node, Ogre::Real a_size,
                                                 Ogre::Real a_time, usint a_ferocity)
{
    scene_node = particle_node;
    lifetime = 0;
    lifetime_limit = a_time + 0.25;

    explosion = Game::scene->createParticleSystem(10, "particles");
    explosion->setMaterialName("explosion");
    explosion->setEmitting(true);

    string size = Game::realIntoString(1);

    explosion->setParameter("particle_width", size);
    explosion->setParameter("particle_height", size);

    Ogre::ParticleAffector* explosion_scaler = explosion->addAffector("Scaler");

    explosion_scaler->setParameter("rate", Game::realIntoString(a_size / a_time));

    Ogre::ParticleAffector* explosion_rotator = explosion->addAffector("Rotator");

    explosion_rotator->setParameter("rotation_speed_range_start", "0");
    explosion_rotator->setParameter("rotation_speed_range_end", "90");
    explosion_rotator->setParameter("rotation_range_start", "0");
    explosion_rotator->setParameter("rotation_range_end", "360");

    Ogre::ParticleEmitter* explosion_emitter = explosion->addEmitter("Ellipsoid");

    explosion_emitter->setParameter("width", size);
    explosion_emitter->setParameter("height", size);
    explosion_emitter->setParameter("depth", size);

    explosion_emitter->setAngle(Ogre::Radian(pi));
    explosion_emitter->setEmissionRate((a_time / 0.25) * a_ferocity);
    explosion_emitter->setMinParticleVelocity(1);
    explosion_emitter->setMaxParticleVelocity(4);
    explosion_emitter->setTimeToLive(a_time);
    explosion_emitter->setDuration(0.25);

    scene_node->attachObject(explosion);
}

ParticleEffectExplosion::~ParticleEffectExplosion()
{
    Game::scene->destroyParticleSystem(explosion);
    die();
}

int ParticleEffectExplosion::update(Ogre::Real a_dt)
{
    lifetime += a_dt;
    if (lifetime > lifetime_limit)
        return 1;

    return 0;
}

