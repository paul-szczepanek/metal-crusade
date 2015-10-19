#ifndef ENTITY_H
#define ENTITY_H

#include "main.h"

class Corpus;
class Collision;

class ArenaEntity
{
public:
  ArenaEntity();
  ArenaEntity(const string& a_name);
  virtual ~ArenaEntity();

  virtual const Vector3& getXYZ();
  virtual Real getX();
  virtual Real getY();
  virtual Real getZ();
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
};

inline void ArenaEntity::setName(const string& a_name)
{
  Name = a_name;
}

inline const string& ArenaEntity::getName()
{
  return Name;
}

inline const Vector3& ArenaEntity::getXYZ()
{
  return Vector3::ZERO;
}

inline Real ArenaEntity::getX()
{
  return getXYZ().x;
}

inline Real ArenaEntity::getY()
{
  return getXYZ().y;
}

inline Real ArenaEntity::getZ()
{
  return getXYZ().z;
}

inline void ArenaEntity::setXYZ(const Vector3& /*a_pos*/)
{
}

inline bool ArenaEntity::handleCollision(Collision* /*a_collision*/)
{
  return true;
}

#endif // ENTITY_H
