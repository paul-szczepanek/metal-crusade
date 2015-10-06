// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "particle_effect_flipbook.h"
#include "game.h"

ParticleEffectFlipbook::ParticleEffectFlipbook(Ogre::SceneNode* a_scene_node,
                                               Real             a_size,
                                               Real             a_ferocity,
                                               fx_anim::anims   a_animation)
  : current_frame(0)
{
  lifetime = 0;
  scene_node = a_scene_node;

  // create the set to hold the anim and attach to the handed down node
  set = Game::Scene->createBillboardSet(1);
  scene_node->attachObject(set);

  switch (a_animation) {
  default: {
    num_frames = 8;
    lifetime_limit = 1.f / num_frames;

    set->setMaterialName("explosion_petrol");
    set->setTextureStacksAndSlices(2, 4);

    set->setDefaultDimensions(0.32 * a_size, 0.64 * a_size);

    billboard = set->createBillboard(Vector3(0, 0.32 * a_size, 0));
  }
  }

}

ParticleEffectFlipbook::~ParticleEffectFlipbook()
{
  // we need to remove the scene node first
  die();
  // then we can safely kill the set
  Game::Scene->destroyBillboardSet(set);
}

bool ParticleEffectFlipbook::update(const Real a_dt)
{
  lifetime += a_dt;

  if (lifetime > lifetime_limit) {
    // flip to next frame
    ++current_frame;

    // if this wasn't the last frame create a billboard
    if (current_frame < num_frames) {
      // reset the timer
      lifetime = 0;

      // change the billboards
      billboard->setTexcoordIndex(current_frame);

    } else {

      // and finish the anim
      return false;
    }
  }

  return true;
}
