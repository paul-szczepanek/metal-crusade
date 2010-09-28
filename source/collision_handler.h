//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef COLLISIONHANDLER_H
#define COLLISIONHANDLER_H

#include "main.h"

class Corpus;
class Collision;

struct collision_pair {
    collision_pair(Corpus* a_object1, Corpus* a_object2)
        : object1(a_object1), object2(a_object2) { };
    Corpus* object1;
    Corpus* object2;
};

class CollisionHandler
{
public:
    CollisionHandler();
    ~CollisionHandler();

    //main loop
    void update(Ogre::Real a_dt);

    //functions hooking up objects into the system
    void registerMobilis(Corpus* a_mobilis);
    void deregisterMobilis(Corpus* a_mobilis);

private:
    //main loop
    void updatePotentialCollisions();
    void evaluatePotentialCollisions();

    //collisions being processed
    vector<collision_pair> possible_collisions;
    vector<Collision*> collisions;

    //all registered objects
    list<Corpus*> registered_objects;

    //temp
    Ogre::Vector2 arena_size;
};

#endif // COLLISIONHANDLER_H
