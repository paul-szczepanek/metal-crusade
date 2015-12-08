// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "weapon.h"
#include "unit.h"
#include "projectile_factory.h"
#include "game_controller.h"
#include "files_handler.h"
#include "projectile.h"
#include "game.h"
#include "game_arena.h"
#include "text_store.h"

Weapon::Weapon(const string& a_name,
               Unit*         a_unit,
               Vector3       a_position,
               usint         a_extra_ammo)
  : Timeout(0), ProjecitlesPrimed(0), Owner(a_unit), XYZ(a_position)
{
  // load spec from file
  if (loadWeaponDesign(a_name) == false) {
    Game::kill(a_name + " weapon spec garbled! Oh, dear.");
  }

  // prime the ammo number from the spec
  Ammo = weapon_design.ammo_per_slot + weapon_design.ammo_per_slot * a_extra_ammo;
}

/** @brief load weapon spec from file
 */
bool Weapon::loadWeaponDesign(const string& filename)
{
  using namespace FilesHandler;

  //prepare map to read data into
  map<string, string> pairs;
  //insert data from file into pairs
  assert(getPairs(filename, WEAPON_DIR, pairs));

  //fill structs with info from pairs
  weapon_design.filename = filename;
  weapon_design.model = pairs["weapon_design.model"];
  //game text
  weapon_design.text_name = Game::Text->getStringKey(pairs["weapon_design.text_name"]);
  weapon_design.text_list_name = Game::Text->getStringKey(pairs["weapon_design.text_list_name"]);
  weapon_design.text_description =
    Game::Text->getStringKey(pairs["weapon_design.text_description"]);
  //dimensions
  weapon_design.type = weapon_type(getEnum(pairs["weapon_design.type"]));
  weapon_design.weight = getReal(pairs["weapon_design.weight"]);
  weapon_design.internals = intoInt(pairs["weapon_design.internals"]);
  weapon_design.panels = intoInt(pairs["weapon_design.panels"]);
  //multifire weapons
  weapon_design.multi_fire = intoInt(pairs["weapon_design.multi_fire"]);
  weapon_design.multi_fire_timout = getReal(pairs["weapon_design.multi_fire_timout"]);
  //basic properties
  weapon_design.spread = getReal(pairs["weapon_design.spread"]) * pi;
  weapon_design.recharge_time = getReal(pairs["weapon_design.recharge_time"]);
  weapon_design.heat_generated = getReal(pairs["weapon_design.heat_generated"]);
  weapon_design.muzzle_velocity = getReal(pairs["weapon_design.muzzle_velocity"]);
  weapon_design.range = getReal(pairs["weapon_design.range"]);
  weapon_design.ammo_per_slot = intoInt(pairs["weapon_design.ammo_per_slot"]);
  //damage
  weapon_design.HeatDmg = getReal(pairs["weapon_design.heat_dmg"]);
  weapon_design.BallisticDmg = getReal(pairs["weapon_design.ballistic_dmg"]);
  weapon_design.penetration = getReal(pairs["weapon_design.penetration"]);
  weapon_design.EnergyDmg = getReal(pairs["weapon_design.energy_dmg"]);
  //exploading damage
  weapon_design.splash_range = getReal(pairs["weapon_design.splash_range"]);
  weapon_design.splash_velocity = getReal(pairs["weapon_design.splash_velocity"]);
  //projectile
  weapon_design.projectile = pairs["weapon_design.projectile"];
  weapon_design.projectile_weight = getReal(pairs["weapon_design.projectile_weight"]);
  //used mainly for missiles
  weapon_design.fuel = getReal(pairs["weapon_design.fuel"]);
  weapon_design.homing = getReal(pairs["weapon_design.homing"]);
  weapon_design.lock_on_time = getReal(pairs["weapon_design.lock_on_time"]);

  return true;
}

/** @brief update the timeout and fire weapon if primed
 */
void Weapon::update(const Real a_dt)
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
      Quaternion orientation = Owner->getOrientation();
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
      Projectile* new_projectile = Game::Projectile->spawnProjectile(weapon_position,
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

    if ((*it)->OwnerWeapon == NULL) { // remove if expired
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
