// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PROJECTILE_H_INCLUDED
#define PROJECTILE_H_INCLUDED

class Weapon;
class Corpus;
class Collision;

#include "corpus.h"
#include "dynamic_entity.h"

/** \class Projectile doesn't need a GameController and controls the Corpus itself according
 *  according to weapon specs
 */
class Projectile
  : public DynamicEntity
{
public:
  Projectile();
  virtual ~Projectile();
  void reset(Weapon* a_weapon = NULL,
             Corpus* a_corpus = NULL);

  bool update(const Real a_dt);

  virtual const Vector3& getXYZ();
  virtual void setXYZ(const Vector3& a_pos);

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
  Vector3 Velocity = Vector3::ZERO;
  // for splash damage
  bool Exploading;
};

inline void Projectile::setOwner(Weapon* a_owner)
{
  OwnerWeapon = a_owner;
}

#endif // PROJECTILE_H_INCLUDED
