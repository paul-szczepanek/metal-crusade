// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PARTICLEEFFECTFLIPBOOK_H
#define PARTICLEEFFECTFLIPBOOK_H

#include "particle_effect.h"

namespace fx_anim
{
enum anims {
  petrol,
};

};

class ParticleEffectFlipbook
  : public ParticleEffect
{
public:
  ParticleEffectFlipbook(Ogre::SceneNode* a_scene_node,
                         Real             a_size,
                         Real             a_ferocity,
                         fx_anim::anims   a_animation);
  ~ParticleEffectFlipbook();

  bool update(const Real a_dt);

private:
  Ogre::BillboardSet* BillboardSet;
  Ogre::Billboard* Billboard;
  size_t NumFrames;
  size_t CurrentFrame;

};

#endif // PARTICLEEFFECTFLIPBOOK_H
