//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "game.h"
#include "collision_handler.h"
#include "collision.h"
#include "corpus.h"

CollisionHandler::CollisionHandler()
{
    //this should prepare the buckets when and if I get to that
}

CollisionHandler::~CollisionHandler()
{
    //destroy collisions if there are any left not dealt with
    for (ulint i = 0, for_size = collisions.size(); i < for_size; ++i) {
        delete (collisions[i]);
    }
}

/** @brief put the dynamic object into the collision system
  */
void CollisionHandler::registerMobilis(Corpus* a_mobilis)
{
    registered_objects.push_back(a_mobilis);
}

/** @brief remove the dynamic object from the collision system
  */
void CollisionHandler::deregisterMobilis(Corpus* a_mobilis)
{
    registered_objects.remove(a_mobilis);
}



/** @brief fills a list of potential collisions based on bounding spheres
  */
void CollisionHandler::updatePotentialCollisions()
{
    //clear last frame collisions if any
    possible_collisions.clear();

    list<Corpus*>::iterator it = registered_objects.begin();
    list<Corpus*>::iterator it_end = registered_objects.end();

    //don't do last one as there would be nothing to intersect with
    for (; it != it_end; ++it) {
        //goes through all elements in list and gets bounding spheres
        Sphere sphere1 = (*it)->getBoundingSphere();

        //check against all except the ones before
        list<Corpus*>::iterator it2 = it;
        ++it2;

        for (; it2 != registered_objects.end(); ++it2) {
            Sphere sphere2 = (*it2)->getBoundingSphere();

            //check if bounding spehres intersect
            if (sphere1.intersects(sphere2)) {
                //validate whether the collision is possible
                if ((*it)->validateCollision((*it2)) && (*it2)->validateCollision((*it))) {
                    //stick it in the list of potential collisions
                    possible_collisions.push_back(collision_pair(*it, *it2));
                }
            }
        }
    }
}

/** @brief check for collision based on individual spheres
  * @todo: profile whether it pays off to use exclusion spheres
  * though they are needed if I want to have flat surfaces
  */
void CollisionHandler::evaluatePotentialCollisions()
{
    //go through all list of potential collisions
    for (ulint i = 0, for_size = possible_collisions.size(); i < for_size; ++i) {
        //reset collision so that first hit creates a new one rather than just append spheres to one
        bool new_collision = true;

        //for convenience - might optimise out later
        Corpus* object1 = possible_collisions[i].object1;
        Corpus* object2 = possible_collisions[i].object2;

        //see how many spheres there are in object
        usint num_of_cs1 = object1->collision_spheres.size();
        usint num_of_cs2 = object2->collision_spheres.size();

        //two objects will multiple collision spheres colliding
        if (num_of_cs1 > 0 && num_of_cs2 > 0) {
            //get bounding spheres
            Sphere bounding_sphere1 = object1->getBoundingSphere();
            Sphere bounding_sphere2 = object2->getBoundingSphere();

            //eliminate excluding spheres by bounding spheres
            bitset<max_num_es> es_bitset2 = object2->getExclusionSpheres(bounding_sphere1);

            //get possible collision spheres minus sets from excluding spheres
            bitset<max_num_cs> cs_bitset1 = object1->getCollisionSpheres(bounding_sphere2);

            //check possible collision spheres from one against the other
            if (cs_bitset1.any()) {
                for (usint j = 0; j < num_of_cs1; ++j) {
                    //if the sphere is not excluded
                    if (cs_bitset1[j]) {
                        //get the valid collision sphere
                        Sphere sphere1 = object1->collision_spheres[j];

                        //and use it to eliminate by exclusion from the other set
                        bitset<max_num_cs> cs_bitset2 = object2->getCollisionSpheres(sphere1,
                                                        es_bitset2);

                        for (usint k = 0; k < num_of_cs2; ++k) {
                            if (cs_bitset2[k]) { //if it hasn't been excluded
                                //finally check for intersetcion
                                if (sphere1.intersects(object2->collision_spheres[k])) {

                                    if (new_collision) { //add collision if it's first in loop
                                        new_collision = false;
                                        collisions.push_back(new Collision(object1, object2));
                                    }

                                    //add sphere indices to collision
                                    collisions.back()->addCollisionSpheres(j, k);
                                }
                            }
                        }
                    }
                }
            }
        } else if (num_of_cs1 == 0 && num_of_cs2 == 0) {
            //two object with only bounding spheres, do they collide?
            if (object1->getBoundingSphere().intersects(object2->getBoundingSphere())) {
                //then add a collision, no collision spehres needed
                collisions.push_back(new Collision(object1, object2));
            }
        } else {
            //only one of them doesn't have collision spheres
            if (num_of_cs1 == 0) {
                //if the first doesn't have any collision spheres flip the objects around
                object1 = possible_collisions[i].object2;
                object2 = possible_collisions[i].object1;
                num_of_cs1 = num_of_cs2; //only num_of_cs1 is used so no need for a proper flip
            }

            Sphere bounding_sphere2 = object2->getBoundingSphere();

            //get possible collision spheres based on the bounding sphere
            bitset<max_num_cs> cs_bitset1 = object1->getCollisionSpheres(bounding_sphere2);

            //check possible collision spheres from one against the bounding sphere
            for (usint j = 0; j < num_of_cs1; ++j) {
                //if the sphere is not excluded
                if (cs_bitset1[j]) {
                    if (bounding_sphere2.intersects(object1->collision_spheres[j])) {
                        if (new_collision) { //add collision if it's first in loop
                            new_collision = false;
                            collisions.push_back(new Collision(object1, object2));
                        }

                        //add sphere indices to collision
                        collisions.back()->addCollisionSpheres(j);
                    }
                }
            }
        }
    }
}

/** @brief produce collisions and evaluate them
  */
void CollisionHandler::update(Ogre::Real a_dt)
{
    updatePotentialCollisions();
    evaluatePotentialCollisions();

    //resolve each collision
    for (ulint i = 0, for_size = collisions.size(); i < for_size; ++i) {
        collisions[i]->resolve(a_dt);
        //and remove it form the list
        delete (collisions[i]);
    }

    //and clear the list
    collisions.clear();
}
