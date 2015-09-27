// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "tank.h"
// yeah, this one is waaaays off
// basically it's for all the mooks on the battlefield that will act as cannon fodder
// small tanks, transports, civilians etc.

Tank::Tank(Vector3          a_pos_xyz,
           const string&    a_unit_name,
           Ogre::SceneNode* a_scene_node,
           Quaternion       a_orientation)
  : Unit(a_unit_name, a_pos_xyz, a_orientation)
{
}
