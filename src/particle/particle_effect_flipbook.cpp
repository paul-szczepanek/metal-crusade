// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "particle_effect_flipbook.h"
#include "game.h"

ParticleEffectFlipbook::ParticleEffectFlipbook(Ogre::SceneNode* a_scene_node,
                                               Real             a_size,
                                               Real             a_ferocity,
                                               fx_anim::anims   a_animation)
  : CurrentFrame(0)
{
  CurrentLifetime = 0;
  SceneNode = a_scene_node;

  // create the set to hold the anim and attach to the handed down node
  BillboardSet = Game::Scene->createBillboardSet(1);
  SceneNode->attachObject(BillboardSet);

  switch (a_animation) {
  default: {
    NumFrames = 8;
    LifetimeLimit = 1.f / NumFrames;

    BillboardSet->setMaterialName("explosion_petrol");
    BillboardSet->setTextureStacksAndSlices(2, 4);

    BillboardSet->setDefaultDimensions(0.32 * a_size, 0.64 * a_size);

    Billboard = BillboardSet->createBillboard(Vector3(0, 0.32 * a_size, 0));
  }
  }

}

ParticleEffectFlipbook::~ParticleEffectFlipbook()
{
  // we need to remove the scene node first
  die();
  // then we can safely kill the set
  Game::Scene->destroyBillboardSet(BillboardSet);
}

bool ParticleEffectFlipbook::update(const Real a_dt)
{
  CurrentLifetime += a_dt;

  if (CurrentLifetime > LifetimeLimit) {
    // flip to next frame
    ++CurrentFrame;

    // if this wasn't the last frame create a billboard
    if (CurrentFrame < NumFrames) {
      // reset the timer
      CurrentLifetime = 0;

      // change the billboards
      Billboard->setTexcoordIndex(CurrentFrame);

    } else {

      // and finish the anim
      return false;
    }
  }

  return true;
}
