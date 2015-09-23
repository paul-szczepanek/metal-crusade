// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef WEAPON_H
#define WEAPON_H

#include "main.h"

class Unit;

enum weapon_type {
  weapon_type_cannon,
  weapon_type_launcher_missile,
  weapon_type_energy,
  weapon_type_launcher
};

struct weapon_design_t {
  string filename;
  string model;
  weapon_type type;
  sint multi_fire;
  Ogre::Real multi_fire_timout;
  Ogre::Real spread;
  Ogre::Real weight;
  sint internals;
  sint panels;
  Ogre::Real recharge_time;
  Ogre::Real heat_generated;
  Ogre::Real muzzle_velocity;
  Ogre::Real heat_dmg;
  Ogre::Real ballistic_dmg;
  Ogre::Real penetration;
  Ogre::Real energy_dmg;
  Ogre::Real splash_range;
  Ogre::Real splash_velocity;
  string projectile;
  Ogre::Real projectile_weight;
  Ogre::Real fuel;
  Ogre::Real homing;
  Ogre::Real lock_on_time;
  uint ammo_per_slot;
  Ogre::Real range;
  // strings
  ulint text_name;
  ulint text_list_name;
  ulint text_description;
};

class Weapon
{
public:
  Weapon(const string& a_name, Unit* a_unit, Ogre::Vector3 a_position, usint a_extra_ammo);
  virtual ~Weapon() { };

  // main loop
  void update(Ogre::Real a_dt);

  bool fire();

  // for hud
  ulint getAmmo() { return ammo / (weapon_design.multi_fire + 1); };
  Ogre::Real getCharge() { return 1 - (timeout / weapon_design.recharge_time); };
  bool isOperational() { return (ammo > 0); };

  weapon_design_t weapon_design;

private:
  ulint ammo;
  Ogre::Real timeout;
  uint projecitles_primed;
  Ogre::Real timeout_step;

  // parent unit
  Unit* unit;

  // this is temp, this should create a scene node, attach it to the torso model
  // and then use that position to determine the origin of projectiles.
  Ogre::Vector3 position;

};

#endif // WEAPON_H
