// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef COLLISION_H
#define COLLISION_H

#include "main.h"
#include "collision_type.h"

class Corpus;

struct reverse_pairs_t {
  size_t a;
  size_t b;
  int sign;
};

const reverse_pairs_t PAIRS[2] = { { 0, 1, 1 }, { 1, 0, -1 } };

class Collision
{
public:
  Collision(Corpus*            a_object1,
            Corpus*            a_object2,
            bitset<MAX_NUM_CS> a_cs_bitset1,
            bitset<MAX_NUM_CS> a_cs_bitset2,
            Real               a_depth);
  ~Collision();

  void findCollisionPlane();
  void resolve(Real a_dt);

  // resulting velocity after the collision
  Vector3 getVelocity();
  Corpus* getCollidingObject();
  vector<usint> getCollisionSphereIndexes();

  void setHitConductivity(Real a_conductivity);

  // colliding objects
  Corpus* Object[2];
  size_t ObjectHits[2];
  Vector3 Centre[2];
  Vector3 Velocity[2];
  vector<Collision*>* CollisionGroup[2] = { NULL, NULL };
  Real Depth;
  size_t resolved = 0;

  // collision qualities
  collision_type ResultType;

private:
  // colliding spheres
  bitset<MAX_NUM_CS> CSIndexes[2];

  Real CollisionSpeed;
  Real Conductivity;

  // marker to determine which object is being processed atm
  bool HandlingFirstObject;
};

Corpus* Collision::getCollidingObject()
{
  return HandlingFirstObject ? Object[0] : Object[1];
}

/** @brief returns the corrected velocity to the appropriate colliding object
 */
inline Vector3 Collision::getVelocity()
{
  return HandlingFirstObject ? Velocity[0] : Velocity[1];
}

void Collision::setHitConductivity(Real a_conductivity)
{
  if (a_conductivity < Conductivity) {
    Conductivity = a_conductivity;
  }
}

#endif // COLLISION_H
