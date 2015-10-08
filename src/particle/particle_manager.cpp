// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "particle_manager.h"
#include "particle_effect.h"
#include "particle_effect_explosion.h"
#include "particle_effect_flipbook.h"
#include "particle_effect_step_dust.h"
#include "game.h"

ParticleManager::ParticleManager()
{
}

ParticleManager::~ParticleManager()
{
  for (list<ParticleEffect*>::iterator it = Effects.begin(); it != Effects.end(); ++it) {
    delete *it;
  }

  Effects.clear();
}

void ParticleManager::update(const Real a_dt)
{
  // call update on every effect
  for (auto it = Effects.begin(); it != Effects.end(); ) {
    // remove if expired
    if (!(*it)->update(a_dt)) {
      delete *it;

      // get the iterator to the next item after removal
      it = Effects.erase(it);

    } else {
      ++it;
    }
  }
}

ParticleEffect* ParticleManager::createExplosion(Vector3 a_position,
                                                 Real    a_size,
                                                 Real    a_time,
                                                 usint   a_ferocity)
{
  Ogre::SceneNode* particle_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  particle_node->setPosition(a_position);

  // effects.push_back(new ParticleEffectExplosion(particle_node, a_size, a_time, a_ferocity));
  // temp
  Effects.push_back(new ParticleEffectFlipbook(particle_node, a_size,
                                               a_ferocity, fx_anim::petrol));

  return Effects.back();
}

ParticleEffect* ParticleManager::createExplosion(Ogre::SceneNode* scene_node,
                                                 Real             a_size,
                                                 Real             a_time,
                                                 usint            a_ferocity)
{
  Ogre::SceneNode* particle_node = scene_node->createChildSceneNode();

  Effects.push_back(new ParticleEffectExplosion(particle_node, a_size, a_time, a_ferocity));

  return Effects.back();
}

ParticleEffect* ParticleManager::createStepDust(Ogre::SceneNode* scene_node)
{
  Ogre::SceneNode* particle_node = scene_node->createChildSceneNode();

  Effects.push_back(new ParticleEffectStepDust(particle_node));

  return Effects.back();
}
