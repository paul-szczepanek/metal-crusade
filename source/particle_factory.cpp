//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "particle_factory.h"
#include "particle_effect.h"
#include "particle_effect_explosion.h"
#include "particle_effect_step_dust.h"
#include "game.h"

ParticleFactory::ParticleFactory()
{
}

ParticleFactory::~ParticleFactory()
{
    for (list<ParticleEffect*>::iterator it = effects.begin(); it != effects.end(); ++it) {
        delete *it;
    }

    effects.clear();
}

void ParticleFactory::update(Ogre::Real a_dt)
{
    //call update on every unit
    for (list<ParticleEffect*>::iterator it = effects.begin(); it != effects.end(); ) {
        //remove if expired
        if ((*it)->update(a_dt) == 1) {
            delete *it;

            //get the iterator to the next item after removal
            it = effects.erase(it);

        } else {
            ++it;
        }
    }
}



ParticleEffect* ParticleFactory::createExplosion(Ogre::Vector3 a_position, Ogre::Real a_size,
                                                 Ogre::Real a_time, usint a_ferocity)
{
    Ogre::SceneNode* particle_node = Game::scene->getRootSceneNode()->createChildSceneNode();
    particle_node->setPosition(a_position);

    effects.push_back(new ParticleEffectExplosion(particle_node, a_size, a_time, a_ferocity));

    return effects.back();
}

ParticleEffect* ParticleFactory::createExplosion(Ogre::SceneNode* scene_node, Ogre::Real a_size,
                                                 Ogre::Real a_time, usint a_ferocity)
{
    Ogre::SceneNode* particle_node = scene_node->createChildSceneNode();

    effects.push_back(new ParticleEffectExplosion(particle_node, a_size, a_time, a_ferocity));

    return effects.back();
}

ParticleEffect* ParticleFactory::createStepDust(Ogre::SceneNode* scene_node)
{
    Ogre::SceneNode* particle_node = scene_node->createChildSceneNode();

    effects.push_back(new ParticleEffectStepDust(particle_node));

    return effects.back();
}
