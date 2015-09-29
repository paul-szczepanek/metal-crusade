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
  Collision(Corpus*                  a_object1,
            Corpus*                  a_object2,
            bitset<MAX_NUM_CS>       a_cs_bitset1,
            bitset<MAX_NUM_CS>       a_cs_bitset2,
            bitset<MAX_NUM_CS_AREAS> a_cs_area_bitset1,
            bitset<MAX_NUM_CS_AREAS> a_cs_area_bitset2,
            Real                     a_depth);
  ~Collision();

  void findCollisionPlane();
  void resolve(Real a_dt);

  // resulting velocity after the collision
  Vector3 getVelocity();
  Corpus* getCollidingObject();
  vector<usint> getCollisionSphereIndexes(size_t a_index);
  vector<usint> getAreasIndexes(size_t a_index);

  void setHitConductivity(Real a_conductivity);

public:
  // colliding objects
  Corpus* Object[2];
  Vector3 Centre[2];
  vector<Collision*>* CollisionGroup[2] = { NULL, NULL };

  // colliding spheres
  bitset<MAX_NUM_CS> CSIndexes[2];
  bitset<MAX_NUM_CS_AREAS> CSAreas[2];

  size_t resolved = 0;

  // collision qualities
  Real Depth;
  collision_type ResultType;
};

#endif // COLLISION_H
