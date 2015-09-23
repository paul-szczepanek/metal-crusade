#ifndef NAVPOINT_H
#define NAVPOINT_H

#include "main.h"

class NavPoint
{
public:
  NavPoint(Ogre::Vector3 a_pos_xyz, string a_name = "navpoint x");
  virtual ~NavPoint();

private:
  string name;
  Ogre::Vector3 PosXyz;
};

#endif // NAVPOINT_H
