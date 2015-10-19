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

const Vector3& NavPoint::getXYZ()
{
  return XYZ;
}

void NavPoint::setXYZ(const Vector3& a_pos)
{
  XYZ = a_pos;
}
