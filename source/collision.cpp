//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "collision.h"
#include "corpus.h"
#include "sphere.h"

Collision::Collision(Corpus* a_object1, Corpus* a_object2)
    : result_type(collision_type_soft), collision_velocity(0),
    object1(a_object1), object2(a_object2), conductivity1(1), conductivity2(1),
    handling_first_object(true)
{
    velocity1 = object1->getVelocity();
    velocity2 = object2->getVelocity();
}

/** @brief adds a pair of colliding spheres
  */
void Collision::addCollisionSpheres(int a_sphere1, int a_sphere2)
{
    collisions_sphere_indices1.push_back(a_sphere1);
    collisions_sphere_indices2.push_back(a_sphere2);
}

/** @brief adds a collision sphere if the other object colliding only has a bounding sphere
  */
void Collision::addCollisionSpheres(int a_sphere1)
{
    collisions_sphere_indices1.push_back(a_sphere1);
}

/** @brief returns the corrected velocity to the appropriate colliding object
  */
void Collision::setHitConductivity(Ogre::Real a_conductivity)
{
    if (handling_first_object) {
        conductivity1 = a_conductivity;

    } else {
        conductivity2 = a_conductivity;
    }
}

void Collision::resolve(Ogre::Real a_dt)
{
    const Ogre::Real collision_elasticity = 0.2; //collision soft/hard ratio maybe parametrise later
    Ogre::Real weight1 = object1->getWeight();
    Ogre::Real weight2 = object2->getWeight();
    Ogre::Real weight_denominator = 1 / (weight1 + weight2);

    //determine collision type
    collision_type collision1 = object1->getCollisionType();
    collision_type collision2 = object2->getCollisionType();

    if (collision1 == collision_type_impact || collision2 == collision_type_impact) {
        result_type = collision_type_impact; //either impact means impact
    } else if (collision1 == collision_type_blocking || collision2 == collision_type_blocking) {
        result_type = collision_type_blocking; //either blocking means blocking
    } else if (collision1 == collision2) {
        result_type = collision1; //if both hard of both soft it's either soft or hard
    } //if none of these it remains the default soft because one of them is soft

    //perfect inelastic collision with energy preserved
    Ogre::Vector3 combined_velocity = ((weight1 * velocity1)
                                       + (weight2 * velocity2)) * weight_denominator;

    if (result_type == collision_type_impact) {
        //perfect inelastic
        velocity1 = combined_velocity;
        velocity2 = combined_velocity;

    } else {
        //mix elastic and inelastic with a ratio of collision_elasticity
        //calculate perfect elastic collision with energy preserved

        //get colliding spheres or if there aren't any in that object bounding spheres
        Sphere sphere1 = (collisions_sphere_indices1.size() > 0)?
                         object1->collision_spheres[collisions_sphere_indices1[0]]
                         : object1->getBoundingSphere();
        Sphere sphere2 = (collisions_sphere_indices2.size() > 0)?
                         object2->collision_spheres[collisions_sphere_indices2[0]]
                         : object2->getBoundingSphere();

        //vector between sphere centres perpendicular to collision plane
        Ogre::Vector3 collision_vector = (sphere1.centre - sphere2.centre);
        collision_vector.normalise(); //get a unit vector

        //get the scalar velocities perpendicular to the collision plane
        Ogre::Real velocity1_perp = velocity1.dotProduct(collision_vector);
        Ogre::Real velocity2_perp = velocity2.dotProduct(collision_vector);

        //scalar velocities after collision
        Ogre::Real new_velocity1_perp = ((velocity1_perp * (weight1 - weight2)
                                          + 2 * weight2 * velocity2_perp) * weight_denominator);
        Ogre::Real new_velocity2_perp = ((velocity2_perp * (weight2 - weight1)
                                          + 2 * weight1 * velocity1_perp) * weight_denominator);

        //new reflected velocities
        velocity1 = velocity1 - 2 * new_velocity1_perp * collision_vector;
        velocity2 = velocity2 - 2 * new_velocity2_perp * collision_vector;

        //mix elastic and inelastic collisions
        if (result_type == collision_type_soft) {
            //for soft and impact favour inelastic
            combined_velocity *= damping; //damping - take energy out of the system
            velocity1 = velocity1 * collision_elasticity;
            velocity1 += combined_velocity * (1 - collision_elasticity);
            velocity2 = velocity2 * collision_elasticity;
            velocity2 += combined_velocity * (1 - collision_elasticity);

        } else {
            //everything else is hard, favour elastic impact
            velocity1 *= damping; //damping - take energy out of the system
            velocity2 *= damping;
        }

        //push object gently away regardless of angles
        //TODO:revert to last legal position properly
        Ogre::Real depth = sphere1.distanceToSurface(sphere2); //how deep the hit is

        //move them back depending on the weight ratio
        object1->revertMove(collision_vector * depth * weight1 * weight_denominator);
        object2->revertMove(-collision_vector * depth * weight2 * weight_denominator);

        //this is a tmporary cheap cheat to help rotating near obstacles
        velocity1 += collision_vector * repulsion;
        velocity2 += -collision_vector * repulsion;
    }

    //call objects to resolve internal actions
    object1->handleCollision(this);
    handling_first_object = false;
    object2->handleCollision(this);

    //heat exchange (yes, again, I know this is not how the real world works
    //but I don't have a cluster to do real heat physics simulation
    //and I need to bend reality a lot for gameplay reasons, I feel slightly guilty though)
    if (result_type != collision_type_impact) { //if it's an impact heat damage is handled in object
        //get surface temps
        Ogre::Real surface_temperature1 = object1->getSurfaceTemperature();
        Ogre::Real surface_temperature2 = object2->getSurfaceTemperature();

        //temperature difference between two bodies
        Ogre::Real difference = surface_temperature1 - surface_temperature2;

        //appty the difference to both bodies according to their own conductivity
        object1->setSurfaceTemperature(surface_temperature1 - conductivity1 * a_dt * difference);
        object2->setSurfaceTemperature(surface_temperature2 + conductivity2 * a_dt * difference);

        //...and the first law of thermodynamics is crying silently in the corner, sorry
    }
}
