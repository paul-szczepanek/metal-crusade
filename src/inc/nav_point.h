#ifndef NAVPOINT_H
#define NAVPOINT_H

#include "main.h"

class NavPoint
{
public:
  NavPoint(Vector3 a_pos_xyz,
           string  a_name = "navpoint x");
  virtual ~NavPoint();

private:
  string name;
  Vector3 PosXyz;
};

#endif // NAVPOINT_H
