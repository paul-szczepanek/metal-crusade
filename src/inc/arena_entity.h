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
  virtual void setXYZ(const Vector3& a_pos);

  virtual Vector3 getDirection();
  virtual Quaternion getOrientation();

  virtual bool update(const Real a_dt);

  void setName(const string& a_name);
  const string& getName();
  virtual bool handleCollision(Collision* a_collision);

  virtual bool acquireAsTargetBy(ArenaEntity* a_entity);
  virtual void releaseAsTarget(ArenaEntity* a_targeted_by);
  virtual bool loseTarget(ArenaEntity* a_targeted_by,
                          bool         a_forced = false);
  virtual ArenaEntity* getTarget();
  virtual void clearFromTargets();
  virtual Corpus* getGround();

public:
  string Name;
  Vector3 XYZ = Vector3::ZERO;
};

inline void ArenaEntity::setName(const string& a_name)
{
  Name = a_name;
}

inline const string& ArenaEntity::getName()
{
  return Name;
}

inline Vector3 ArenaEntity::getXYZ()
{
  return XYZ;
}

inline void ArenaEntity::setXYZ(const Vector3& a_pos)
{
  XYZ = a_pos;
}

inline bool ArenaEntity::handleCollision(Collision* /*a_collision*/)
{
  return true;
}

#endif // ENTITY_H
