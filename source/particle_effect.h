#ifndef PARTICLEEFFECT_H
#define PARTICLEEFFECT_H

#include "main.h"

class ParticleEffect
{
public:
    ParticleEffect();
    virtual ~ParticleEffect();

    virtual int update(Ogre::Real a_dt) = 0;

protected:
    void die();

    Ogre::SceneNode* scene_node;

    Ogre::Real lifetime;
    Ogre::Real lifetime_limit;
};

#endif // PARTICLEEFFECT_H
