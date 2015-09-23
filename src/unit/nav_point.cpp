#include "nav_point.h"

NavPoint::NavPoint(Vector3 a_pos_xyz,
                   string  a_name)
  : PosXyz(a_pos_xyz), name(a_name)
{
  // ctor
}

NavPoint::~NavPoint()
{
  // dtor
}
