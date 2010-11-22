//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "particle_effect_explosion.h"
#include "game.h"

ParticleEffectExplosion::ParticleEffectExplosion(Ogre::SceneNode* particle_node, Ogre::Real a_size,
                                                 Ogre::Real a_time, Ogre::Real a_ferocity)
{
    scene_node = particle_node;
    lifetime = 0;
    lifetime_limit = a_time + 0.25;

    explosion = Game::scene->createParticleSystem(10, "particles");
    explosion->setSortingEnabled(true);
    explosion->setMaterialName("explosion");
    explosion->setParameter("particle_width", "6");
    explosion->setParameter("particle_height", "1");

    //scale up
    Ogre::ParticleAffector* explosion_scaler = explosion->addAffector("Scaler");
    explosion_scaler->setParameter("rate", Game::realIntoString(a_size / (a_time * 1.25)));

    //rotate
    Ogre::ParticleAffector* explosion_rotator = explosion->addAffector("Rotator");
    explosion_rotator->setParameter("rotation_speed_range_start", "-180");
    explosion_rotator->setParameter("rotation_speed_range_end", "180");
    explosion_rotator->setParameter("rotation_range_start", "0");
    explosion_rotator->setParameter("rotation_range_end", "360");

    //emit from a sphere
    Ogre::ParticleEmitter* explosion_emitter = explosion->addEmitter("Ellipsoid");
    explosion_emitter->setParameter("width", "4");
    explosion_emitter->setParameter("height", "4");
    explosion_emitter->setParameter("depth", "4");
    explosion_emitter->setAngle(Ogre::Radian(pi));
    explosion_emitter->setEmissionRate(a_ferocity * 2);
    explosion_emitter->setMinParticleVelocity(1);
    explosion_emitter->setMaxParticleVelocity(4);
    explosion_emitter->setTimeToLive(a_time);
    explosion_emitter->setDuration(0.25);

    scene_node->attachObject(explosion);

    explosion->setEmitting(true);
}

ParticleEffectExplosion::~ParticleEffectExplosion()
{
    Game::scene->destroyParticleSystem(explosion);
    die();
}

int ParticleEffectExplosion::update(Ogre::Real a_dt)
{
    lifetime += a_dt;

    //kill after lifetime ends
    if (lifetime > lifetime_limit)
        return 1;

    return 0;
}

