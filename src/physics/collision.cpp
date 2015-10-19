// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "collision.h"
#include "corpus.h"
#include "sphere.h"

#define REPULSION_FACTOR (2)
#define HIT_DAMPENING    ((Real)0.9)
#define RESOLVE_LIMIT (10)

Collision::Collision(Corpus*                  a_object1,
                     Corpus*                  a_object2,
                     bitset<MAX_NUM_CS>       a_cs_bitset1,
                     bitset<MAX_NUM_CS>       a_cs_bitset2,
                     bitset<MAX_NUM_CS_AREAS> a_cs_area_bitset1,
                     bitset<MAX_NUM_CS_AREAS> a_cs_area_bitset2,
                     Real                     a_depth)
  : Depth(a_depth)
{
  Object[0] = a_object1;
  Object[1] = a_object2;
  CSIndexes[0] = a_cs_bitset1;
  CSIndexes[1] = a_cs_bitset2;
  CSAreas[0] = a_cs_area_bitset1;
  CSAreas[1] = a_cs_area_bitset2;
}

Collision::~Collision()
{
}

void Collision::findCollisionPlane()
{
  vector<usint> indexes1 = getCollisionSphereIndexes(0);
  Centre[0] = Vector3::ZERO;
  for (const usint i : indexes1) {
    Centre[0] += Object[0]->CollisionSpheres[i].Centre;
  }
  Centre[0] *= 1.0 / indexes1.size();

  vector<usint> indexes2 = getCollisionSphereIndexes(1);
  Centre[1] = Vector3::ZERO;
  for (const usint i : indexes2) {
    Centre[1] += Object[1]->CollisionSpheres[i].Centre;
  }
  Centre[1] *= 1.0 / indexes2.size();
}

void Collision::resolve(const Real a_dt)
{
  // stabilise piles of objects
  if (Resolved > RESOLVE_LIMIT) {
    Object[0]->Velocity = Vector3::ZERO;
    Object[1]->Velocity = Vector3::ZERO;
  }

  Vector3 velocity[2];
  velocity[0] = Object[0]->Velocity;
  velocity[1] = Object[1]->Velocity;

  // only resolve the collision if the objects are getting closer
  const Real distance_before = Object[0]->XYZ.squaredDistance(Object[1]->XYZ);
  const Vector3 pos_later1 = Object[0]->XYZ + velocity[0];
  const Vector3 pos_later2 = Object[1]->XYZ + velocity[1];
  const Real distance_after = pos_later1.squaredDistance(pos_later2);
  if (distance_before < distance_after) {
    return;
  }

  // determine collision type
  collision_type collision_type[2];
  Real weight[2];
  for (const reverse_pairs_t& i : PAIRS) {
    collision_type[i.a] = Object[i.a]->getCollisionType();
    weight[i.a] = (collision_type[i.a] == collision_type_blocking) ?
                  BLOCKING_WEIGHT : Object[i.a]->Weight;
  }

  Real weight_denominator = 1.0 / (weight[0] + weight[1]);

  if (collision_type[0] == collision_type_blocking
      || collision_type[1] == collision_type_blocking) {
    ResultType = collision_type_blocking;
  } else if (collision_type[0] == collision_type_impact || collision_type[1] == collision_type_impact) {
    ResultType = collision_type_impact;
  } else if (collision_type[0] == collision_type[1]) {
    ResultType = collision_type[0];
  } else {
    ResultType = collision_type_soft;
  }

  // vector perpendicular to collision plane pointing towards from the surface
  Vector3 collision_vector = (Centre[1] - Centre[0]);
  collision_vector.normalise();

  // perfect inelastic collision with energy preserved
  Vector3 combined_velocity = Vector3::ZERO;
  Real combined_velocity_perp = 0;

  if (ResultType != collision_type_hard) {
    combined_velocity = ((weight[0] * velocity[0]) + (weight[1] * velocity[1]))
                        * weight_denominator;
    combined_velocity_perp = combined_velocity.dotProduct(collision_vector);
  }

  if (ResultType == collision_type_impact) {
    // perfect inelastic
    velocity[0] = combined_velocity;
    velocity[1] = combined_velocity;

  } else {
    // get the scalar velocities perpendicular to the collision plane
    Real velocity_perp[2];
    Vector3 velocity_par[2];
    velocity_perp[0] = velocity[0].dotProduct(collision_vector);
    velocity_perp[1] = velocity[1].dotProduct(collision_vector);
    velocity_par[0] = velocity[0] - velocity_perp[0] * collision_vector;
    velocity_par[1] = velocity[1] - velocity_perp[1] * collision_vector;
    Real friction_acc = (velocity_par[0] - velocity_par[1]).length()
                        * Object[0]->Friction * Object[1]->Friction;;
    const Real hardness = min(Object[0]->Hardness, Object[1]->Hardness);

    // scalar velocities after collision
    for (const reverse_pairs_t& i : PAIRS) {
      Real new_velocity_perp = (velocity_perp[i.a] * (weight[i.a] - weight[i.b])
                                + 2 * weight[i.b] * velocity_perp[i.b]) * weight_denominator;

      // mix elastic and inelastic collisions if not hard collision
      if (ResultType != collision_type_hard) {
        // remove old perp velocity, add a mix of new perp velocity and combined perp velocity
        new_velocity_perp = (HIT_DAMPENING * new_velocity_perp) * hardness
                            + combined_velocity_perp * (1 - hardness);
      } else {
        new_velocity_perp = (HIT_DAMPENING) * new_velocity_perp;
      }

      // use collision depth to move the objects away if collision velocity is not enough
      Real repulsion = (-i.sign) * Depth * REPULSION_FACTOR * (weight[i.b] * weight_denominator);
      if (new_velocity_perp > repulsion) {
        new_velocity_perp = repulsion;
      }

      if (friction_acc > 0.1) {
        velocity_par[i.a] *= 1 - friction_acc * (weight[i.b] * weight_denominator) * a_dt;
      }
      velocity[i.a] = velocity_par[i.a] + new_velocity_perp * collision_vector;
    }
  }

  // remove the velocity perpendicular to surface of a blocking collision
  for (const reverse_pairs_t& i : PAIRS) {
    if (collision_type[i.a] == collision_type_blocking) {
      Real dot = velocity[i.b].dotProduct(collision_vector);
      if (dot > 0) {
        velocity[i.b] -= i.sign * dot * collision_vector;
      }
    }
  }

  for (const reverse_pairs_t& i : PAIRS) {
    if (collision_type[i.a] != collision_type_blocking) {
      Object[i.a]->Velocity = velocity[i.a];
    }
  }

  if (!Resolved) {
    // heat exchange (yes, again, I know this is not how the real world works
    // but I don't have a beowolf cluster handy to do real heat physics simulation
    // and I need to bend reality a lot for gameplay reasons, I feel slightly guilty though)
    if (ResultType != collision_type_impact) { // if it's an impact heat damage is handled in object
      // get surface temps
      Real surface_temperature1 = Object[0]->SurfaceTemperature;
      Real surface_temperature2 = Object[1]->SurfaceTemperature;

      // temperature difference between two bodies
      Real difference = surface_temperature1 - surface_temperature2;

      Real conductivity = max(Object[0]->Conductivity, Object[1]->Conductivity);

      // TODO: this doesn't work well because of the repulsion hack
      Object[0]->SurfaceTemperature = surface_temperature1 - conductivity * a_dt * difference;
      Object[1]->SurfaceTemperature = surface_temperature2 + conductivity * a_dt * difference;
    }
  }

  // call objects to resolve internal actions
  Object[0]->handleCollision(this);
  Object[1]->handleCollision(this);

  ++Resolved;

  // after changing velocities objects might have to resolve other collisions again
  for (const reverse_pairs_t& i : PAIRS) {
    if (CollisionGroup[i.a]) {
      for (Collision* c : * (CollisionGroup[i.a])) {
        c->resolve(a_dt);
      }
    }
  }
}

/** @brief returns indexes of hit spheres
 */
vector<usint> Collision::getCollisionSphereIndexes(size_t a_index)
{
  vector<usint> indexes;
  for (size_t i = 0; i < Object[a_index]->CollisionSpheres.size(); ++i) {
    if (CSIndexes[a_index][i]) {
      indexes.push_back(i);
    }
  }
  return indexes;
}

/** @brief returns areas indexes of hit spheres
 */
vector<usint> Collision::getAreasIndexes(size_t a_index)
{
  vector<usint> indexes;
  for (size_t i = 0; i < MAX_NUM_CS_AREAS; ++i) {
    if (CSAreas[a_index][i]) {
      indexes.push_back(i);
    }
  }
  return indexes;
}
