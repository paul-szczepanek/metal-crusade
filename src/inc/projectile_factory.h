// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PROJECTILEFACTORY_H
#define PROJECTILEFACTORY_H

#include "factory.h"

class Projectile;
class Weapon;
class Unit;

class ProjectileFactory
  : public Factory
{
public:
  ProjectileFactory();

  ~ProjectileFactory();

  // main loop
  void update(Real a_dt);

  Projectile* getFreeProjectile();
  Corpus* getFreeCorpus();

  // object creation
  void fireProjectile(Vector3    a_pos_xyz,
                      Quaternion a_orientation,
                      Weapon*    a_weapon);

private:
  list<Corpus*> ActiveList;
  vector<Projectile> Projectiles;
  vector<Corpus> Corpuses;
  size_t lastCorpusIdx = 0;
  size_t lastProjectileIdx = 0;
};

#endif // PROJECTILEFACTORY_H
