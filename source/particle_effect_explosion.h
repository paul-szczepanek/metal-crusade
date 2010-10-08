//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef PARTICLEEFFECTEXPLOSION_H
#define PARTICLEEFFECTEXPLOSION_H

#include "particle_effect.h"

class ParticleEffectExplosion : public ParticleEffect
{
public:
    ParticleEffectExplosion(Ogre::SceneNode* scene_node, Ogre::Real a_size, Ogre::Real a_time,
                            usint a_ferocity);
    ~ParticleEffectExplosion();

    int update(Ogre::Real a_dt);

private:
    Ogre::ParticleSystem* explosion;

};

#endif // PARTICLEEFFECTEXPLOSION_H
