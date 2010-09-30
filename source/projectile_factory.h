//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef PROJECTILEFACTORY_H
#define PROJECTILEFACTORY_H

#include "corpus_factory.h"

class Projectile;
class Weapon;
class Unit;

class ProjectileFactory : public CorpusFactory
{
public:
    ProjectileFactory() { };
    ~ProjectileFactory();

    //main loop
    void updateProjectiles(Ogre::Real a_dt);

    //object creation
    void fireProjectile(Ogre::Vector3 a_pos_xyz, Ogre::Quaternion a_orientation,
                        Weapon* a_weapon, Unit* a_owner);

private:
    list<Projectile*> projectile_list;
};

#endif // PROJECTILEFACTORY_H
