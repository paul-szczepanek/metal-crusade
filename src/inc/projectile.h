// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PROJECTILE_H_INCLUDED
#define PROJECTILE_H_INCLUDED

class Weapon;
class Corpus;
class Collision;

#include "corpus.h"
#include "arena_entity.h"

/** \class Projectile doesn't need a GameController and controls the Corpus itself according
 *  according to weapon specs
 */
class Projectile
  : public ArenaEntity
{
public:
  Projectile();
  virtual ~Projectile();
  void reset(Weapon* a_weapon = NULL,
             Corpus* a_corpus = NULL);

  bool update(Real a_dt);

  // handle collision
  virtual bool validateCollision(Corpus* a_collision);
  virtual bool handleCollision(Collision* a_collision);
  void setOwner(Weapon* a_owner);

private:
  // start the explosion if it has splash damage
  void explode();

public:
  Weapon* OwnerWeapon;
  Corpus* Bullet;
  // how far has it traveled
  Real Lifetime;
  Real GracePeriod;
  Vector3 Velocity;
  // for splash damage
  bool Exploading;
};

inline void Projectile::setOwner(Weapon* a_owner)
{
  OwnerWeapon = a_owner;
}

#endif // PROJECTILE_H_INCLUDED
