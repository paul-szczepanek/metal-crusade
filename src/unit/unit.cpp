// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "unit.h"
#include "game.h"
#include "game_arena.h"
#include "game_controller.h"

Unit::Unit(Vector3          a_pos_xyz,
           const string&    a_unit_name,
           Ogre::SceneNode* a_scene_node,
           Quaternion       a_orientation)
  : Corpus(a_pos_xyz, a_unit_name, a_scene_node, a_orientation),
  core_temperature(0), hud_attached(false)
{

}

/** @brief calculate the real target of the weapon and the angle it should fire at
 * @todo: doesn't take into account the drop or air resistance
 */
Quaternion Unit::getBallisticAngle(const Vector3& a_position)
{
  // get the intended target
  Vector3 pointer_position = controller->getPointerPos();
  // get the vector pointing at the inteded target
  Vector3 pointer_direction = pointer_position - a_position;
  // get the flattened vector
  pointer_direction.y = 0;

  // get the real direction confined to the torso angle
  Real distance = pointer_direction.length();
  pointer_direction = getDirection() * distance;
  // get the point the vector is pointing at from the perspective of the weapon
  pointer_position = pointer_direction + a_position;

  // we need to apply the offset so that the angles converge from the weapons mounted on the sides
  Vector3 weapon_offset = pos_xyz - a_position;
  // the angle must be limited otherwise you could fire sideways - this limits it to 5 degrees
  Real limit = distance / 11.4300523;
  if (limit < weapon_offset.length()) {
    weapon_offset.normalise();
    weapon_offset *= limit;
  }
  pointer_position += weapon_offset;

  // check if the real target is within the arena
  Game::Arena->isOutOfArena(pointer_position);
  // get the height at this point
  Real height = Game::Arena->getHeight(pointer_position.x, pointer_position.z);

  // adjust by the target mode
  if (controller->control_block.target_high) {
    pointer_position.y = height + target_high_offset;
  } else if (controller->control_block.target_air) {
    pointer_position.y = height + target_air_offset;
  } else {
    pointer_position.y = height + target_low_offset;
  }

  // the real direction to the real target
  pointer_direction = pointer_position - a_position;

  // return the quaternion for the projectile
  return Vector3::UNIT_Z.getRotationTo(pointer_direction);
}

void Unit::updateTargets()
{
  if (take(controller->control_block.target_pointer)) {
    getUnitAtPointer();
  }
}

/** @brief get the object under the pointer and set as the target
 */
bool Unit::getUnitAtPointer()
{
  Vector3 pointer_position = controller->getPointerPos();
  uint_pair pointer_cell_index = Game::Arena->getCellIndex(pointer_position.x,
                                                           pointer_position.z);
  vector<uint_pair> cell_indexes;
  Game::Arena->getCellIndexesWithinRadius(pointer_cell_index, cell_indexes);

  for (usint i = 0, for_size = cell_indexes.size(); i < for_size; ++i) {
    list<Unit*>& unit_list = Game::Arena->getUnitCell(cell_indexes[i]);

    // if there are any units in the cell
    if (unit_list.size() > 0) {
      list<Unit*>::iterator it = unit_list.begin();
      list<Unit*>::iterator it_end = unit_list.end();

      // walk through all projectiles
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
bool Unit::acquireTarget(Corpus* a_target)
{
  // check if target is acquirable
  if (a_target->acquireAsTarget(this)) {
    // set it as target
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
  // check the position in the arena and update the arena cells if necessary
  out_of_bounds = Game::Arena->updateCellIndex(cell_index, pos_xyz, this);
}

// radar
bool Unit::isDetectable() {
  return true;
}

/** @brief called by targeted object that requires this to relinquish its current target
 * TODO: relinquish criteria
 */
bool Unit::loseTarget(Corpus* a_targeted_by,
                      bool    a_forced)
{
  if (a_forced) {
    target = NULL;
    return true;

  } else {
    // temp!
    target = NULL;
    return true;
  }
}

void Unit::clearFromTargets()
{
  // stop other objects targeting this object
  for (usint i = 0, for_size = target_holders.size(); i < for_size; ++i) {
    target_holders[i]->loseTarget(this, true);
  }

  target_holders.clear();

  // release the target held by this object
  if (target != NULL) {
    target->releaseAsTarget(this);
    target = NULL;
  }
}
