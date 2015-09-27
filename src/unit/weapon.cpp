// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "weapon.h"
#include "unit.h"
#include "projectile_factory.h"
#include "game_controller.h"
#include "files_handler.h"
#include "projectile.h"
#include "game.h"
#include "game_arena.h"

Weapon::Weapon(const string& a_name,
               Unit*         a_unit,
               Vector3       a_position,
               usint         a_extra_ammo)
  : Timeout(0), ProjecitlesPrimed(0), Owner(a_unit), XYZ(a_position)
{
  // load spec from file
  if (FilesHandler::getWeaponDesign(a_name, weapon_design) == false) {
    Game::kill(a_name + " weapon spec garbled! Oh, dear.");
  }

  // prime the ammo number from the spec
  Ammo = weapon_design.ammo_per_slot + weapon_design.ammo_per_slot * a_extra_ammo;
}

/** @brief update the timeout and fire weapon if primed
 */
void Weapon::update(Real a_dt)
{
  if (ProjecitlesPrimed > 0) {
    // this fires each time it passes the multi_fire_timout step
    if (Timeout < TimeoutStep) {
      // take out the fired projectile
      --ProjecitlesPrimed;

      // calculate the next step
      TimeoutStep = weapon_design.recharge_time - (weapon_design.multi_fire
                                                   - ProjecitlesPrimed) *
                    weapon_design.multi_fire_timout;

      // get the orientation of the torso for calulating the position of the projectile spawn
      Quaternion orientation = Owner->getLookingOrientation();
      // get current position of the weapon
      Vector3 weapon_position = Owner->getXYZ() + orientation * XYZ;
      // get the ballistic angle to hit the target
      Quaternion weapon_orientation = Owner->getBallisticAngle(weapon_position);

      // add variation to the angle of firing
      Radian angle_of_spread(Ogre::Math::RangeRandom(-weapon_design.spread,
                                                     weapon_design.spread));
      Quaternion firing_cone = Quaternion(angle_of_spread, Vector3::UNIT_Y);

      // TODO: the spread needs to be in a cone, not in a plane
      // also make sure the spred slice is a circle not a square
      weapon_orientation = firing_cone * weapon_orientation;

      // create projectile
      Projectile* new_projectile = Game::Projectile->fireProjectile(weapon_position,
                                                                    weapon_orientation,
                                                                    this);
      ActiveList.push_back(new_projectile);
    }
  }

  // reduce the timeout to the next fire
  if (Timeout > 0) {
    Timeout -= a_dt;
  }

  // walk through all projectiles and update them
  for (auto it = ActiveList.begin(); it != ActiveList.end(); ) {
    (*it)->update(a_dt);

    if ((*it)->Owner == NULL) { // remove if expired
      // get the iterator to the next item after removal
      it = ActiveList.erase(it);
    } else {
      ++it;
    }
  }
}

/** @brief fire the weapon
 * returns true if the weapon can fire
 */
bool Weapon::fire()
{
  // only fire when previous cycle has finished and there is ammo
  if (Timeout <= 0 && Ammo > 0) {
    // fire the number of projectiles in multifire for as long as there is ammo
    for (size_t i = 0; i < weapon_design.multi_fire && Ammo > 0; ++i) {
      --Ammo; // take one ammo
      ++ProjecitlesPrimed; // set it to launch
    }

    // calculate how long in between shots (for multifire waepons)
    TimeoutStep = weapon_design.recharge_time - (weapon_design.multi_fire
                                                 - ProjecitlesPrimed) *
                  weapon_design.multi_fire_timout;

    // generate heat
    Owner->addHeat(weapon_design.heat_generated);

    // set the timeout before next fire
    Timeout = weapon_design.recharge_time;

    // return that the weapon fired successfully
    return true;

  } else {
    return false;
  }
}
