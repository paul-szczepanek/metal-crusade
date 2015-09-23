// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "weapon.h"
#include "unit.h"
#include "projectile_factory.h"
#include "game_controller.h"
#include "files_handler.h"
#include "game.h"
#include "game_arena.h"

Weapon::Weapon(const string& a_name, Unit* a_unit, Ogre::Vector3 a_position, usint a_extra_ammo)
  : timeout(0), projecitles_primed(0), unit(a_unit), position(a_position)
{
  // load spec from file
  if (FilesHandler::getWeaponDesign(a_name, weapon_design) == false) {
    Game::kill(a_name + " weapon spec garbled! Oh, dear.");
  }

  // prime the ammo number from the spec
  ammo = weapon_design.ammo_per_slot + weapon_design.ammo_per_slot * a_extra_ammo;
}

/** @brief update the timeout and fire weapon if primed
  */
void Weapon::update(Ogre::Real a_dt)
{
  if (projecitles_primed > 0) {
    // this fires each time it passes the multi_fire_timout step
    if (timeout < timeout_step) {
      // take out the fired projectile
      --projecitles_primed;

      // calculate the next step
      timeout_step = weapon_design.recharge_time - (weapon_design.multi_fire
                     - projecitles_primed) * weapon_design.multi_fire_timout;

      // get the orientation of the torso for calulating the position of the projectile spawn
      Ogre::Quaternion orientation = unit->getLookingOrientation();
      // get current position of the weapon
      Ogre::Vector3 weapon_position = unit->getPosition() + orientation * position;
      // get the ballistic angle to hit the target
      Ogre::Quaternion weapon_orientation = unit->getBallisticAngle(weapon_position);

      // add variation to the angle of firing
      Ogre::Radian angle_of_spread(Ogre::Math::RangeRandom(-weapon_design.spread,
                                   weapon_design.spread));
      Ogre::Quaternion firing_cone = Ogre::Quaternion(angle_of_spread, Ogre::Vector3::UNIT_Y);

      // TODO: the spread needs to be in a cone, not in a plane
      // also make sure the spred slice is a circle not a square
      weapon_orientation = firing_cone * weapon_orientation;

      // create projectile
      Game::projectile_factory->fireProjectile(weapon_position, weapon_orientation,
          this, unit);
    }
  }

  // reduce the timeout to the next fire
  if (timeout > 0) {
    timeout -= a_dt;
  }
}

/** @brief fire the weapon
  * returns true if the weapon can fire
  */
bool Weapon::fire()
{
  // only fire when previous cycle has finished and there is ammo
  if (timeout <= 0 && ammo > 0) {
    // fire the number of projectiles in multifire for as long as there is ammo
    for (usint i = 0; i < weapon_design.multi_fire && ammo > 0; ++i) {
      --ammo; // take one ammo
      ++projecitles_primed; // set it to launch
    }

    // calculate how long in between shots (for multifire waepons)
    timeout_step = weapon_design.recharge_time - (weapon_design.multi_fire
                   - projecitles_primed) * weapon_design.multi_fire_timout;

    // generate heat
    unit->addHeat(weapon_design.heat_generated);

    // set the timeout before next fire
    timeout = weapon_design.recharge_time;

    // return that the weapon fired successfully
    return true;

  } else {
    return false;
  }
}
