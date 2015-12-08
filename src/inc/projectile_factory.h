// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PROJECTILEFACTORY_H
#define PROJECTILEFACTORY_H

#include "main.h"

class Projectile;
class Corpus;
class Weapon;
class Unit;

class ProjectileFactory
{
public:
  ProjectileFactory();

  ~ProjectileFactory();

  void update(const Real a_dt);

  Projectile* getFreeProjectile();
  Corpus* getFreeCorpus();

  // object creation
  Projectile* spawnProjectile(Vector3    a_pos_xyz,
                              Quaternion a_orientation,
                              Weapon*    a_weapon);

private:
  vector<Projectile> Projectiles;
  vector<Corpus> Corpuses;
  size_t LastCorpusIdx = 0;
  size_t LastProjectileIdx = 0;
};

#endif // PROJECTILEFACTORY_H
