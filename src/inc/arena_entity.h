#ifndef ENTITY_H
#define ENTITY_H

#include "main.h"

class Corpus;
class Collision;

/** \class ArenaEntity is simple entity meant for non-interactive scenery, it doesn't react
 *  and doesn't control the Corpuses that belong to it
 */
class ArenaEntity
{
public:
  ArenaEntity();
  ArenaEntity(const string& a_name);
  virtual ~ArenaEntity();
  virtual Vector3 getXYZ();
  virtual void setPosition(const Vector3& a_pos);

  void setName(const string& a_name);
  const string& getName();
  virtual bool handleCollision(Collision* a_collision);

public:
  string Name;
  Vector3 XYZ;
  Quaternion Orientation;
};

inline void ArenaEntity::setName(const string& a_name)
{
  Name = a_name;
}

inline const string& ArenaEntity::getName()
{
  Name;
}

inline Vector3 ArenaEntity::getXYZ()
{
  return XYZ;
}

inline void ArenaEntity::setPosition(const Vector3& a_pos)
{
  XYZ = a_pos;
}

#endif // ENTITY_H
