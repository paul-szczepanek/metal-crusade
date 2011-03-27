//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "unit.h"
#include "game.h"
#include "arena.h"
#include "game_controller.h"

Unit::Unit(Ogre::Vector3 a_pos_xyz, const string& a_unit_name,
           Ogre::SceneNode* a_scene_node, Ogre::Quaternion a_orientation)
    : Mobilis(a_pos_xyz, a_unit_name, a_scene_node, a_orientation),
    core_temperature(0), hud_attached(false)
{

}

/** @brief calculate the real target of the weapon and the angle it should fire at
  * @todo: doesn't take into account the drop or air resistance
  */
Ogre::Quaternion Unit::getBallisticAngle(const Ogre::Vector3& a_position)
{
    //get the intended target
    Ogre::Vector3 pointer_position = controller->getPointerPos();
    //get the vector pointing at the inteded target
    Ogre::Vector3 pointer_direction = pointer_position - a_position;
    //get the flattened vector
    pointer_direction.y = 0;

    //get the real direction confined to the torso angle
    Ogre::Real distance = pointer_direction.length();
    pointer_direction = getDirection() * distance;
    //get the point the vector is pointing at from the perspective of the weapon
    pointer_position = pointer_direction + a_position;

    //we need to apply the offset so that the angles converge from the weapons mounted on the sides
    Ogre::Vector3 weapon_offset = pos_xyz - a_position;
    //the angle must be limited otherwise you could fire sideways - this limits it to 5 degrees
    Ogre::Real limit = distance / 11.4300523;
    if (limit < weapon_offset.length()) {
        weapon_offset.normalise();
        weapon_offset *= limit;
    }
    pointer_position += weapon_offset;

    //check if the real target is within the arena
    Game::arena->isOutOfArena(pointer_position);
    //get the height at this point
    Ogre::Real height = Game::arena->getHeight(pointer_position.x, pointer_position.z);

    //adjust by the target mode
    if (controller->control_block.target_high) {
        pointer_position.y = height + target_high_offset;
    } else if (controller->control_block.target_air) {
        pointer_position.y = height + target_air_offset;
    } else {
        pointer_position.y = height + target_low_offset;
    }

    //the real direction to the real target
    pointer_direction = pointer_position - a_position;

    //return the quaternion for the projectile
    return Ogre::Vector3::UNIT_Z.getRotationTo(pointer_direction);
}

void Unit::updateTargets()
{
    if (Game::take(controller->control_block.target_pointer)) {
        getUnitAtPointer();
    }
}

/** @brief get the object under the pointer and set as the target
  */
bool Unit::getUnitAtPointer()
{
    Ogre::Vector3 pointer_position = controller->getPointerPos();
    uint_pair pointer_cell_index = Game::arena->getCellIndex(pointer_position.x,
                                                             pointer_position.z);
    vector<uint_pair> cell_indices;
    Game::arena->getCellIndicesWithinRadius(pointer_cell_index, cell_indices);

    for (usint i = 0, for_size = cell_indices.size(); i < for_size; ++i) {
        list<Unit*>& unit_list = Game::arena->getUnitCell(cell_indices[i]);

        //if there are any units in the cell
        if (unit_list.size() > 0) {
            list<Unit*>::iterator it = unit_list.begin();
            list<Unit*>::iterator it_end = unit_list.end();

            //walk through all projectiles
            for (; it != it_end; ++it) {
                if ((*it)->getBoundingSphere().contains(pointer_position)) {
                    return acquireTarget((*it));
                }
            }
        }
    }

    return false;
}

/** @brief get the passed in mobilis as target
  */
bool Unit::acquireTarget(Mobilis* a_target)
{
    //check if target is acquirable
    if (a_target->acquireAsTarget(this)) {
        //set it as target
        target = a_target;

        return true;
    }

    return false;
}

/** @brief puts the object on the arean and in the correct array and in the correct cell index
  * updates the cell index and position if out of bounds
  */
void Unit::updateCellIndex()
{
    //check the position in the arena and update the arena cells if necessary
    out_of_bounds = Game::arena->updateCellIndex(cell_index, pos_xyz, this);
}
