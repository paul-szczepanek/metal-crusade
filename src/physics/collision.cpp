// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "collision.h"
#include "corpus.h"
#include "sphere.h"

#define REPULSION_FACTOR (2)
#define HIT_DAMPENING    (0.9)

Collision::Collision(Corpus*            a_object1,
                     Corpus*            a_object2,
                     bitset<MAX_NUM_CS> a_cs_bitset1,
                     bitset<MAX_NUM_CS> a_cs_bitset2,
                     Real               a_depth)
  : Depth(a_depth),
  Conductivity(1),
  ResultType(collision_type_soft),
  HandlingFirstObject(true)
{
  Object[0] = a_object1;
  Object[1] = a_object2;
  CSIndexes[0] = a_cs_bitset1;
  CSIndexes[1] = a_cs_bitset2;
  Velocity[0] = Object[0]->getVelocity();
  Velocity[1] = Object[1]->getVelocity();
  CollisionSpeed = (Velocity[0] - Velocity[1]).length();
}

void Collision::findCollisionPlane()
{
  vector<usint> indexes1 = getCollisionSphereIndexes();
  for (const usint& i : indexes1) {
    Centre[0] += Object[0]->CollisionSpheres[i].Centre;
  }
  Centre[0] *= 1.0 / indexes1.size();

  HandlingFirstObject = false;
  vector<usint> indexes2 = getCollisionSphereIndexes();
  for (const usint& i : indexes2) {
    Centre[1] += Object[1]->CollisionSpheres[i].Centre;
  }
  Centre[1] *= 1.0 / indexes2.size();
  HandlingFirstObject = true;
}

#define BLOCKING_WEIGHT (10000000)
#define RESOLVE_LIMIT (10)

void Collision::resolve(Real a_dt)
{
  // stabilise piles of objects
  if (resolved > RESOLVE_LIMIT) {
    Object[0]->Velocity = Vector3::ZERO;
    Object[1]->Velocity = Vector3::ZERO;
  }

  Vector3 velocity[2];
  velocity[0] = Object[0]->Velocity;
  velocity[1] = Object[1]->Velocity;

  // only resolve the collision if the objects are getting closer
  const Real distance_before = Object[0]->XYZ.squaredDistance(Object[1]->XYZ);
  const Real distance_after = (Object[0]->XYZ + velocity[0]).squaredDistance(
    Object[1]->XYZ + velocity[1]);
  if (distance_before > distance_after) {
    return;
  }

  // vector perpendicular to collision plane pointing away from the surface
  Vector3 collision_vector = (Centre[0] - Centre[1]);
  collision_vector.normalise(); // get a unit vector

  const Real hardness = min(Object[0]->Hardness, Object[1]->Hardness);

  // determine collision type
  collision_type collision_type[2];
  Real weight[2];
  for (const reverse_pairs_t& i : PAIRS) {
    collision_type[i.a] = Object[i.a]->getCollisionType();
    weight[i.a] = (collision_type[i.a] == collision_type_blocking) ?
                  BLOCKING_WEIGHT : Object[i.a]->Weight;
  }

  Vector3 velocity_impact[2];
  // remove the velocity perpendicular to surface of a blocking collision
  for (const reverse_pairs_t& i : PAIRS) {
    if (collision_type[i.a] != collision_type_blocking) {
      Real dot = velocity[i.a].dotProduct(collision_vector);
      velocity_impact[i.a] -= i.sign * dot * collision_vector;
    }
  }

  Real weight_denominator = 1.0 / (weight[0] + weight[1]);

  // perfect inelastic collision with energy preserved
  Vector3 combined_velocity = Vector3::ZERO;
  Vector3 combined_velocity_impact = Vector3::ZERO;

  if (collision_type[0] == collision_type_blocking || collision_type[1] ==
      collision_type_blocking) {
    ResultType = collision_type_blocking;

  } else {
    if (collision_type[0] == collision_type_impact || collision_type[1] == collision_type_impact) {
      ResultType = collision_type_impact;
    } else if (collision_type[0] == collision_type[1]) {
      ResultType = collision_type[0];
    } else {
      // if none of these it remains the default soft because one of them is soft
      // ResultType = collision_type_soft;
    }

    if (ResultType != collision_type_hard) {
      combined_velocity = ((weight[0] * velocity[0]) + (weight[1] * velocity[1]))
                          * weight_denominator;
      combined_velocity_impact = combined_velocity.dotProduct(collision_vector) * collision_vector;
    }
  }

  // remove the velocity perpendicular to surface of a blocking collision
  for (const reverse_pairs_t& i : PAIRS) {
    if (collision_type[i.a] == collision_type_blocking) {
      Vector3 velocity_impact = (Centre[i.a] - Centre[i.b]);
      Real dot = velocity[i.b].dotProduct(collision_vector);
      if (dot > 0) {
        velocity[i.b] -= i.sign * dot * collision_vector;
      }
    }
  }

  if (ResultType == collision_type_impact) {
    // perfect inelastic
    velocity[0] = combined_velocity;
    velocity[1] = combined_velocity;

  } else {

    // get the scalar velocities perpendicular to the collision plane
    Real velocity_perp[2];
    velocity_perp[0] = velocity[0].dotProduct(collision_vector);
    velocity_perp[1] = velocity[1].dotProduct(collision_vector);

    // scalar velocities after collision
    Real new_velocity_perp[2];
    for (const reverse_pairs_t& i : PAIRS) {
      new_velocity_perp[i.a] = ((velocity_perp[i.a] * (weight[i.a] - weight[i.b])
                                 + 2 * weight[i.b] * velocity_perp[i.b]) * weight_denominator);
      // new reflected velocities
      velocity[i.a] = velocity[i.a] - 2 * new_velocity_perp[i.a] * collision_vector;

      // mix elastic and inelastic collisions
      if (ResultType != collision_type_hard) {
        velocity[i.a] = velocity[i.a] * hardness + combined_velocity_impact * (1 - hardness);
      }

      // push object gently away regardless of angles
      velocity[i.a] += i.sign * collision_vector * Depth * REPULSION_FACTOR
                       * (1 - weight[i.a] * weight_denominator);

      // damping - take energy out of the system
      velocity[i.a] *= HIT_DAMPENING;

    }

    // this is a temporary cheap cheat to help rotating near obstacles
    /*velocity[0] += collision_vector * REPULSION_FACTOR;
       velocity[1] += -collision_vector * REPULSION_FACTOR;*/
  }

  Object[0]->Velocity = velocity[0];
  Object[1]->Velocity = velocity[1];

  if (!resolved) {
    // call objects to resolve internal actions
    Object[0]->handleCollision(this);
    HandlingFirstObject = false;
    Object[1]->handleCollision(this);

    // heat exchange (yes, again, I know this is not how the real world works
    // but I don't have a beowolf cluster handy to do real heat physics simulation
    // and I need to bend reality a lot for gameplay reasons, I feel slightly guilty though)
    if (ResultType != collision_type_impact) { // if it's an impact heat damage is handled in object
      // get surface temps
      Real surface_temperature1 = Object[0]->SurfaceTemperature;
      Real surface_temperature2 = Object[1]->SurfaceTemperature;

      // temperature difference between two bodies
      Real difference = surface_temperature1 - surface_temperature2;

      // TODO: this doesn't work well because of the repulsion hack
      Object[0]->SurfaceTemperature = surface_temperature1 - Conductivity * a_dt * difference;
      Object[1]->SurfaceTemperature = surface_temperature2 + Conductivity * a_dt * difference;
    }
  }

  ++resolved;

  // after changing velocities objects might have to resolve other collisions again
  for (const reverse_pairs_t& i : PAIRS) {
    if (CollisionGroup[i.a]) {
      for (Collision* c : *(CollisionGroup[i.a])) {
        c->resolve(a_dt);
      }
    }
  }
}

/** @brief returns indexes of hit spheres
 */
vector<usint> Collision::getCollisionSphereIndexes()
{
  vector<usint> indexes;
  const bitset<MAX_NUM_CS>& cs_set =  HandlingFirstObject ? CSIndexes[0] : CSIndexes[1];
  Corpus* corpus = HandlingFirstObject ? Object[1] : Object[0];
  for (size_t i = 0; i < corpus->CollisionSpheres.size(); ++i) {
    if (cs_set[i]) {
      indexes.push_back(i);
    }
  }
  return indexes;
}
