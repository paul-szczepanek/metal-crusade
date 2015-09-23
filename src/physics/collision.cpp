// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "collision.h"
#include "corpus.h"
#include "sphere.h"

static const Ogre::Real repulsion = 2;
static const Ogre::Real damping = 0.9;

Collision::Collision(Corpus* a_object1, Corpus* a_object2)
  : ResultType(collision_type_soft), CollisionVelocity(0),
    Object1(a_object1), Object2(a_object2), Conductivity1(1), Conductivity2(1),
    HandlingFirstObject(true)
{
  Velocity1 = Object1->getVelocity();
  Velocity2 = Object2->getVelocity();
}

/** @brief adds a pair of colliding spheres
  */
void Collision::addCollisionSpheres(int a_sphere1, int a_sphere2)
{
  CSIndexes1.push_back(a_sphere1);
  CSIndexes2.push_back(a_sphere2);
}

/** @brief adds a collision sphere if the other object colliding only has a bounding sphere
  */
void Collision::addCollisionSpheres(int a_sphere1)
{
  CSIndexes1.push_back(a_sphere1);
}

/** @brief returns the corrected velocity to the appropriate colliding object
  */
void Collision::setHitConductivity(Ogre::Real a_conductivity)
{
  if (HandlingFirstObject) {
    Conductivity1 = a_conductivity;

  } else {
    Conductivity2 = a_conductivity;
  }
}

void Collision::resolve(Ogre::Real a_dt)
{
  const Ogre::Real collision_elasticity = 0.2; // collision soft/hard ratio maybe parametrise later
  Ogre::Real weight1 = Object1->getWeight();
  Ogre::Real weight2 = Object2->getWeight();
  Ogre::Real weight_denominator = 1 / (weight1 + weight2);

  // determine collision type
  collision_type collision1 = Object1->getCollisionType();
  collision_type collision2 = Object2->getCollisionType();

  if (collision1 == collision_type_impact || collision2 == collision_type_impact) {
    ResultType = collision_type_impact; // either impact means impact
  } else if (collision1 == collision_type_blocking || collision2 == collision_type_blocking) {
    ResultType = collision_type_blocking; // either blocking means blocking
  } else if (collision1 == collision2) {
    ResultType = collision1; // if both hard of both soft it's either soft or hard
  } // if none of these it remains the default soft because one of them is soft

  // perfect inelastic collision with energy preserved
  Ogre::Vector3 combined_velocity = ((weight1 * Velocity1)
                                     + (weight2 * Velocity2)) * weight_denominator;

  if (ResultType == collision_type_impact) {
    // perfect inelastic
    Velocity1 = combined_velocity;
    Velocity2 = combined_velocity;

  } else {
    // mix elastic and inelastic with a ratio of collision_elasticity
    // calculate perfect elastic collision with energy preserved

    // get colliding spheres or if there aren't any in that object bounding spheres
    Sphere sphere1 = (CSIndexes1.size() > 0) ?
                     Object1->CollisionSpheres[CSIndexes1[0]]
                     : Object1->getBoundingSphere();
    Sphere sphere2 = (CSIndexes2.size() > 0) ?
                     Object2->CollisionSpheres[CSIndexes2[0]]
                     : Object2->getBoundingSphere();

    // vector between sphere centres perpendicular to collision plane
    Ogre::Vector3 collision_vector = (sphere1.Centre - sphere2.Centre);
    collision_vector.normalise(); // get a unit vector

    if (ResultType == collision_type_blocking) {
      // hit a concrete wall - instant stop
      Velocity1 = 0;
      Velocity2 = 0;

    } else {
      // get the scalar velocities perpendicular to the collision plane
      Ogre::Real velocity1_perp = Velocity1.dotProduct(collision_vector);
      Ogre::Real velocity2_perp = Velocity2.dotProduct(collision_vector);

      // scalar velocities after collision
      Ogre::Real new_velocity1_perp = ((velocity1_perp * (weight1 - weight2)
                                        + 2 * weight2 * velocity2_perp) * weight_denominator);
      Ogre::Real new_velocity2_perp = ((velocity2_perp * (weight2 - weight1)
                                        + 2 * weight1 * velocity1_perp) * weight_denominator);

      // new reflected velocities
      Velocity1 = Velocity1 - 2 * new_velocity1_perp * collision_vector;
      Velocity2 = Velocity2 - 2 * new_velocity2_perp * collision_vector;

      // mix elastic and inelastic collisions
      if (ResultType == collision_type_soft) {
        // for soft and impact favour inelastic
        Velocity1 = Velocity1 * collision_elasticity;
        Velocity1 += combined_velocity * (1 - collision_elasticity);
        Velocity2 = Velocity2 * collision_elasticity;
        Velocity2 += combined_velocity * (1 - collision_elasticity);

      } else {
        // everything else is hard, favour elastic impact
        // which is rleady calculated TODO: handle blocking here better - it can't move
      }
    }

    // push object gently away regardless of angles
    // TODO:revert to last legal position properly
    Ogre::Real depth = sphere1.distanceToSurface(sphere2); // how deep the hit is

    // move them back depending on the weight ratio
    if (Object1->revertMove(collision_vector * depth * weight1 * weight_denominator)) {
      Velocity1 *= -damping; // damping - take energy out of the system
    }
    if (Object2->revertMove(-collision_vector * depth * weight2 * weight_denominator)) {
      Velocity2 *= -damping; // damping - take energy out of the system
    }

    // this is a tmporary cheap cheat to help rotating near obstacles
    Velocity1 += collision_vector * repulsion;
    Velocity2 += -collision_vector * repulsion;
  }

  // call objects to resolve internal actions
  Object1->handleCollision(this);
  HandlingFirstObject = false;
  Object2->handleCollision(this);

  // heat exchange (yes, again, I know this is not how the real world works
  // but I don't have a beowolf cluster handy to do real heat physics simulation
  // and I need to bend reality a lot for gameplay reasons, I feel slightly guilty though)
  if (ResultType != collision_type_impact) { // if it's an impact heat damage is handled in object
    // get surface temps
    Ogre::Real surface_temperature1 = Object1->getSurfaceTemperature();
    Ogre::Real surface_temperature2 = Object2->getSurfaceTemperature();

    // temperature difference between two bodies
    Ogre::Real difference = surface_temperature1 - surface_temperature2;

    // appty the difference to both bodies according to their own conductivity
    // TODO: this doesn't work well beacuse of the repulsion hack
    Object1->setSurfaceTemperature(surface_temperature1 - Conductivity1 * a_dt * difference);
    Object2->setSurfaceTemperature(surface_temperature2 + Conductivity2 * a_dt * difference);
    // ...and the first law of thermodynamics is crying silently in the corner, sorry
  }
}
