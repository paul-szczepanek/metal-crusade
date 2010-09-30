//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "weapon.h"
#include "unit.h"
#include "projectile_factory.h"
#include "game_controller.h"
#include "files_handler.h"
#include "game.h"
#include "arena.h"

Weapon::Weapon(const string& a_name, Unit* a_unit, Ogre::Vector3 a_position, usint a_extra_ammo)
    : timeout(0), projecitles_primed(0), unit(a_unit), position(a_position)
{
    //load spec from file
    if (FilesHandler::getWeaponSpec(a_name, weapon_spec) == false)
        Game::kill(a_name+" weapon spec garbled! Oh, dear.");

    //prime the ammo number from the spec
    ammo = weapon_spec.ammo_per_slot + weapon_spec.ammo_per_slot * a_extra_ammo;
}

/** @brief update the timeout and fire weapon if primed
  */
void Weapon::update(Ogre::Real a_dt)
{
    if (projecitles_primed > 0) {
        //this fires each time it passes the multi_fire_timout step
        if (timeout < timeout_step) {
            //take out the fired projectile
            --projecitles_primed;

            //calculate the next step
            timeout_step = weapon_spec.recharge_time - (weapon_spec.multi_fire
                           - projecitles_primed) * weapon_spec.multi_fire_timout;

            //get the orientation of the torso for calulating the position of the projectile spawn
            Ogre::Quaternion orientation = unit->getLookingOrientation();
            //get current position of the weapon
            Ogre::Vector3 weapon_position = unit->getPosition() + orientation * position;
            //get the ballistic angle to hit the target
            Ogre::Quaternion weapon_orientation = unit->getBallisticAngle(weapon_position);

            //create projectile
            Game::projectile_factory->fireProjectile(weapon_position, weapon_orientation,
                                                     this, unit);
        }
    }

    //reduce the timeout to the next fire
    if (timeout > 0) {
        timeout -= a_dt;
    }
}

/** @brief fire the weapon
  * returns true if the weapon can fire
  */
bool Weapon::fire()
{
    //only fire when previous cycle has finished and there is ammo
    if (timeout <= 0 && ammo > 0) {
        //fire the number of projectiles in multifire for as long as there is ammo
        for (usint i = 0; i < weapon_spec.multi_fire && ammo > 0; ++i) {
            --ammo; //take one ammo
            ++projecitles_primed; //set it to launch
        }

        //calculate how long in between shots (for multifire waepons)
        timeout_step = weapon_spec.recharge_time - (weapon_spec.multi_fire
                                  - projecitles_primed) * weapon_spec.multi_fire_timout;

        //generate heat
        unit->addHeat(weapon_spec.heat_generated);

        //set the timeout before next fire
        timeout = weapon_spec.recharge_time;

        //return that the weapon fired successfully
        return true;

    } else {
        return false;
    }
}
