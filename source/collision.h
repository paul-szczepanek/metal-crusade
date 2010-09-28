//Copyright 2010 Paul Szczepanek. Code released under GPLv3

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

    //main loop
    void resolve(Ogre::Real a_dt);

    //spheres added by the collision handler
    void addCollisionSpheres(int a_sphere1, int a_sphere2);
    void addCollisionSpheres(int a_sphere1);

    //resulting velocity after the collision
    Ogre::Vector3 getVelocity();

    //get spheres that actually colllided on the calling object
    vector<usint> getCollisionSphereIndeces();

    //if the objects requires to inspect the other colliding object directly
    Corpus* getCollidingObject() { return handling_first_object? object2 : object1; };

    //called by handled objects to report their effective conductivity
    void setHitConductivity(Ogre::Real a_conductivity);

private:
    //collision qualities
    collision_type result_type;
    Ogre::Real collision_velocity;

    //colliding objects
    Corpus* object1;
    Corpus* object2;

    Ogre::Vector3 velocity1;
    Ogre::Vector3 velocity2;
    Ogre::Real conductivity1;
    Ogre::Real conductivity2;

    //colliding spheres
    vector<usint> collisions_sphere_indices1;
    vector<usint> collisions_sphere_indices2;

    //marker to determine which object is being processed atm
    bool handling_first_object;
};

const Ogre::Real repulsion = 2;
const Ogre::Real damping = 0.9;

/** @brief returns the spheres on the calling object that were hit in the collision
  */
inline vector<usint> Collision::getCollisionSphereIndeces() {
    return handling_first_object? collisions_sphere_indices1 : collisions_sphere_indices2;
}

/** @brief returns the corrected velocity to the appropriate colliding object
  */
inline Ogre::Vector3 Collision::getVelocity()
{
    return handling_first_object? velocity1 : velocity2;
}

#endif // COLLISION_H
