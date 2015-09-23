// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef COLLISION_H
#define COLLISION_H

#include "main.h"
#include "collision_type.h"

class Corpus;

class Collision
{
public:
  Collision(Corpus* a_object1, Corpus* a_object2);
  ~Collision() { };

  // main loop
  void resolve(Ogre::Real a_dt);

  // spheres added by the collision handler
  void addCollisionSpheres(int a_sphere1, int a_sphere2);
  void addCollisionSpheres(int a_sphere1);

  // resulting velocity after the collision
  Ogre::Vector3 getVelocity();

  // get spheres that actually colllided on the calling object
  vector<usint> getCollisionSphereIndexes();

  // if the objects requires to inspect the other colliding object directly
  Corpus* getCollidingObject() { return HandlingFirstObject ? Object2 : Object1; };

  // called by handled objects to report their effective conductivity
  void setHitConductivity(Ogre::Real a_conductivity);

private:
  // collision qualities
  collision_type ResultType;
  Ogre::Real CollisionVelocity;

  // colliding objects
  Corpus* Object1;
  Corpus* Object2;

  Ogre::Vector3 Velocity1;
  Ogre::Vector3 Velocity2;
  Ogre::Real Conductivity1;
  Ogre::Real Conductivity2;

  // colliding spheres
  vector<usint> CSIndexes1;
  vector<usint> CSIndexes2;

  // marker to determine which object is being processed atm
  bool HandlingFirstObject;
};

/** @brief returns the spheres on the calling object that were hit in the collision
  */
inline vector<usint> Collision::getCollisionSphereIndexes()
{
  return HandlingFirstObject ? CSIndexes1 : CSIndexes2;
}

/** @brief returns the corrected velocity to the appropriate colliding object
  */
inline Ogre::Vector3 Collision::getVelocity()
{
  return HandlingFirstObject ? Velocity1 : Velocity2;
}

#endif // COLLISION_H
