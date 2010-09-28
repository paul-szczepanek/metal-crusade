//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef TANK_H
#define TANK_H

#include "unit.h"

class Tank : public Unit
{
public:
    Tank(Ogre::Vector3 a_pos_xyz, const string& a_unit_name, Ogre::SceneNode* a_scene_node,
         Ogre::Quaternion a_orientation)
        : Unit::Unit(a_pos_xyz, a_unit_name, a_scene_node, a_orientation) { };
    ~Tank();
private:
};

#endif // TANK_H
