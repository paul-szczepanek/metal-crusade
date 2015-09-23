// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PROJECTILE_H_INCLUDED
#define PROJECTILE_H_INCLUDED

class Weapon;
class Collision;

#include "corpus.h"

class Projectile
  : public ArenaEntity
{
public:
  Projectile(Vector3          a_pos_xyz,
             const string&    a_unit_name,
             Ogre::SceneNode* a_scene_node,
             Quaternion       a_orientation,
             Weapon*          a_weapon,
             Corpus*          a_owner);
  ~Projectile() {
  }

  // main loop
  int update();

  // handle collision
  virtual bool validateCollision(Corpus* a_collision);
  virtual int handleCollision(Collision* a_collision);

private:
  // start the explosion if it has splash damage
  void explode();

public:
  Weapon* weapon;
  size_t NumCorpuses;

  // how far has it traveled
  Real lifetime;

  // for splash damage
  Real coverage;

  // madness
  Real velocity_dmg_multiplier

  bool exploading;
};

#endif // PROJECTILE_H_INCLUDED
