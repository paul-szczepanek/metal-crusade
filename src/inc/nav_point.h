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
};

#endif // NAVPOINT_H
