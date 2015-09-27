#include "nav_point.h"

NavPoint::NavPoint(Vector3 a_pos_xyz,
                   string  a_name)
  : ArenaEntity(a_name)
{
  XYZ = a_pos_xyz;
}

NavPoint::~NavPoint()
{
  // dtor
}
