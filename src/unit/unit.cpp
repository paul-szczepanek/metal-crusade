// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "unit.h"
#include "game.h"
#include "game_arena.h"
#include "game_controller.h"

Unit::Unit(const string& a_unit_name)
{
  Name = a_unit_name;
}

Unit::~Unit()
{
  clearFromTargets();
}

/** @brief calculate the real target of the weapon and the angle it should fire at
 * @todo: doesn't take into account the drop or air resistance
 */
Quaternion Unit::getBallisticAngle(const Vector3& a_position)
{
  // get the intended target
  Vector3 pointer_position = Controller->getPointerPos();
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
  Vector3 weapon_offset = getXYZ() - a_position;
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
  if (Controller->ControlBlock.target_high) {
    pointer_position.y = height + target_high_offset;
  } else if (Controller->ControlBlock.target_air) {
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
  if (take(Controller->ControlBlock.target_pointer)) {
    getUnitAtPointer();
  }
}

/** @brief get the object under the pointer and set as the target
 */
bool Unit::getUnitAtPointer()
{
  const Vector3& pointer_position = Controller->getPointerPos();
  vector<size_t_pair> cell_indexes;
  Game::Arena->getCellIndexesWithinRadius(pointer_position, cell_indexes);

  for (size_t i = 0, for_size = cell_indexes.size(); i < for_size; ++i) {
    list<Corpus*>& CorpusList = Game::Arena->getCorpusCell(cell_indexes[i]);

    // if there are any units in the cell
    for (Corpus* c : CorpusList) {
      if (c->BoundingSphere.contains(pointer_position)) {
        return acquireTarget(c);
      }
    }
  }

  return false;
}

bool Unit::acquireTarget(Corpus* a_target)
{
  // check if target is acquirable
  if (a_target->OwnerEntity && a_target->OwnerEntity->acquireAsTargetBy(this)) {
    // set it as target
    CurrentTarget = static_cast<Unit*>(a_target->OwnerEntity);

    return true;
  }

  return false;
}

/** @brief called by an object which holds this as a target to tell it that it no longer targets it
 */
void Unit::releaseAsTarget(Unit* a_targeted_by)
{
  if (TargetHolders.size() > 0) {
    vector<Unit*>::iterator it = find(TargetHolders.begin(),
                                      TargetHolders.end(), a_targeted_by);
    if (it < TargetHolders.end()) {
      TargetHolders.erase(it);
    }
  }
}

/** @brief called by other object to try and acquire this as a target
 * return false if target can't be acquired
 */
bool Unit::acquireAsTargetBy(Unit* a_targeted_by)
{
  TargetHolders.push_back(a_targeted_by);

  return true;
}

// radar
bool Unit::isDetectable()
{
  return true;
}

/** @brief called by targeted object that requires this to relinquish its current target
 * TODO: relinquish criteria
 */
bool Unit::loseTarget(Unit* a_targeted_by,
                      bool  a_forced)
{
  if (a_forced) {
    CurrentTarget = NULL;
    return true;

  } else {
    // temp!
    CurrentTarget = NULL;
    return true;
  }
}

void Unit::clearFromTargets()
{
  // stop other objects targeting this object
  for (size_t i = 0, for_size = TargetHolders.size(); i < for_size; ++i) {
    TargetHolders[i]->loseTarget(this, true);
  }

  TargetHolders.clear();

  // release the target held by this object
  if (CurrentTarget != NULL) {
    CurrentTarget->releaseAsTarget(this);
    CurrentTarget = NULL;
  }
}

// orientation
Quaternion Unit::getDriveOrientation()
{
  return Quaternion::IDENTITY;
}

Vector3 Unit::getDriveDirection()
{
  return Vector3::ZERO;
}
