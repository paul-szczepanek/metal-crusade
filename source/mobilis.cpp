//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "mobilis.h"
#include "collision_handler.h"
#include "game.h"
#include "arena.h"

Mobilis::Mobilis(Ogre::Vector3 a_pos_xyz, const string& a_unit_name,
                             Ogre::SceneNode* a_scene_node, Ogre::Quaternion a_orientation)
    : Corpus::Corpus(a_pos_xyz, a_unit_name, a_scene_node, a_orientation),
    velocity(Ogre::Vector3(0, 0, 0)), move(Ogre::Vector3(0, 0, 0)), corrected_velocity_scalar(0),
    out_of_bounds(false), registered(false), core_integrity(1), target(NULL)
{
    orientation = scene_node->getOrientation();
}

Mobilis::~Mobilis()
{
    clearFromTargets();
}

/** @brief handleCollision
  */
int Mobilis::handleCollision(Collision* a_collision)
{
    scene_node->showBoundingBox(true);//temp

    return 0;
}

/** @brief called by an object which holds this as a target to tell it that it no longer targets it
  */
void Mobilis::releaseAsTarget(Mobilis* a_targeted_by)
{
    if (target_holders.size() > 0) {
        vector<Mobilis*>::iterator it = find(target_holders.begin(),
                                                   target_holders.end(), a_targeted_by);
        if (it < target_holders.end()) {
            target_holders.erase(it);
        }
    }
}

/** @brief called by other object to try and aqcuire this as a target
  * return false if target can't be aqcuired
  */
bool Mobilis::acquireAsTarget(Mobilis* a_targeted_by)
{
    target_holders.push_back(a_targeted_by);

    return true;
}

/** @brief called by targeted object that requires this to relinquish its current target
  * TODO: relinquish criteria
  */
bool Mobilis::loseTarget(Mobilis* a_targeted_by, bool a_forced)
{
    if (a_forced) {
        target = NULL;
        return true;

    } else {
        //temp!
        target = NULL;
        return true;
    }
}

void Mobilis::clearFromTargets()
{
    //stop other objects targeting this object
    for (usint i = 0, for_size = target_holders.size(); i < for_size; ++i) {
        target_holders[i]->loseTarget(this , true);
    }

    target_holders.clear();

    //release the target held by this object
    if (target != NULL) {
        target->releaseAsTarget(this);
        target = NULL;
    }
}

/** @brief reverts an illegal move
  * @todo: replace by something less painfully simplistic
  */
bool Mobilis::revertMove(Ogre::Vector3 a_move)
{
    pos_xyz -= a_move;//haha only serious

    return true;
}

int Mobilis::updateController()
{
    return 0;
}

int Mobilis::update(Ogre::Real a_dt)
{
    updateCellIndex();

    //clear last frame collisions
    collided_with.clear();

    //chain the corpus update
    int return_code = Corpus::update(a_dt);

    //update the scene node
    scene_node->setPosition(pos_xyz);
    scene_node->setOrientation(orientation);

    //mark collision spheres as moved
    bounding_sphere_invalid = true;
    es_invalid.set();
    cs_invalid.set();

    //collision system hookup
    if (return_code == 0) { //if it's not dead
        if (!registered) { //and not registered yet
            registered = true; //register it with the collision system
            Game::collider->registerMobilis(this);
        }
    } else { //if it's dead
        if (registered) { //remove from collisin checking
            Game::collider->deregisterMobilis(this);
            Game::arena->purgeCellIndex(cell_index, this);
        }
    }

    return return_code;
}

/** @brief puts the object on the arean and in the correct array and in the correct cell index
  * updates the cell index and position if out of bounds
  * by the magic of virtual tables it knows which array to use
  */
void Mobilis::updateCellIndex()
{
    //check the position in the arena and update the arena cells if necessary
    out_of_bounds = Game::arena->updateCellIndex(cell_index, pos_xyz, this);
}
