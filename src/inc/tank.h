// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef TANK_H
#define TANK_H

#include "unit.h"

class Tank
  : public Unit
{
public:
  Tank(Vector3          a_pos_xyz,
       const string&    a_unit_name,
       Ogre::SceneNode* a_scene_node,
       Quaternion       a_orientation);
  virtual ~Tank();
private:
};

#endif // TANK_H
