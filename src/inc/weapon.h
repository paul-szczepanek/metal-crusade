// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef WEAPON_H
#define WEAPON_H

#include "main.h"

class Unit;
class Projectile;

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
  Real multi_fire_timout;
  Real spread;
  Real weight;
  sint internals;
  sint panels;
  Real recharge_time;
  Real heat_generated;
  Real muzzle_velocity;
  Real HeatDmg;
  Real BallisticDmg;
  Real penetration;
  Real EnergyDmg;
  Real splash_range;
  Real splash_velocity;
  string projectile;
  Real projectile_weight;
  Real fuel;
  Real homing;
  Real lock_on_time;
  size_t ammo_per_slot;
  Real range;
  // strings
  ulint text_name;
  ulint text_list_name;
  ulint text_description;
};

class Weapon
{
public:
  Weapon(const string& a_name,
         Unit*         a_unit,
         Vector3       a_position,
         usint         a_extra_ammo);
  virtual ~Weapon()
  {
  }

  void update(Real a_dt);

  bool fire();

  // for hud
  ulint getAmmo()
  {
    return Ammo / (weapon_design.multi_fire + 1);
  }

  Real getCharge()
  {
    return 1 - (Timeout / weapon_design.recharge_time);
  }

  bool isOperational()
  {
    return (Ammo > 0);
  }

  weapon_design_t weapon_design;

public:
  // parent unit
  Unit* Owner;

private:
  ulint Ammo;
  Real Timeout;
  size_t ProjecitlesPrimed;
  Real TimeoutStep;

  Vector3 XYZ;

  list<Projectile*> ActiveList;
};

#endif // WEAPON_H
