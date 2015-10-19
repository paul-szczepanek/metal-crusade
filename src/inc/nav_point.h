#ifndef NAVPOINT_H
#define NAVPOINT_H

#include "main.h"
#include "arena_entity.h"

class NavPoint
  : public ArenaEntity
{
public:
  NavPoint(Vector3 a_pos_xyz,
           string  a_name = "navpoint x");
  virtual ~NavPoint();

  virtual const Vector3& getXYZ();
  virtual void setXYZ(const Vector3& a_pos);
public:
  Vector3 XYZ;
};

#endif // NAVPOINT_H
