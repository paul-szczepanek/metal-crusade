#ifndef PARTICLEFACTORY_H
#define PARTICLEFACTORY_H

#include "corpus_factory.h"

class ParticleEffect;

class ParticleFactory : public CorpusFactory
{
public:
    ParticleFactory();
    virtual ~ParticleFactory();

    void update(Ogre::Real a_dt);

    //create explosions
    ParticleEffect* createExplosion(Ogre::SceneNode* scene_node, Ogre::Real a_size,
                                    Ogre::Real a_time, usint a_ferocity = 1);
    ParticleEffect* createExplosion(Ogre::Vector3 a_position, Ogre::Real a_size,
                                    Ogre::Real a_time, usint a_ferocity = 1);

private:
    list<ParticleEffect*> effects;
};

#endif // PARTICLEFACTORY_H
