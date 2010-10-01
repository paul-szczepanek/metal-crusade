//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef PROJECTILE_H_INCLUDED
#define PROJECTILE_H_INCLUDED

class Weapon;
class Collision;

#include "mobilis.h"

class Projectile : public Mobilis
{
public:
    Projectile(Ogre::Vector3 a_pos_xyz, const string& a_unit_name, Ogre::SceneNode* a_scene_node,
               Ogre::Quaternion a_orientation, Weapon* a_weapon, Corpus* a_owner);
    ~Projectile() { };

    //main loop
    int updateController();

    //resolves collision
    int handleCollision(Collision* a_collision);

    //damage
    Ogre::Real getBallisticDmg();
    Ogre::Real getEnergyDmg();
    Ogre::Real getHeatDmg();

    //radar
    bool isDetectable() { return false; };

    //handle collision
    bool validateCollision(Corpus* a_collision);

private:
    Corpus* owner;
    Weapon* weapon;

    //how far has it travelled
    Ogre::Real lifetime;

    //for splash damage
    bool exploading;
    Ogre::Real coverage;

    //madness
    Ogre::Real velocity_dmg_multiplier;
};

#endif // PROJECTILE_H_INCLUDED
