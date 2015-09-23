// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CRUSADER_DESIGN_H_INCLUDED
#define CRUSADER_DESIGN_H_INCLUDED

#include "armour_type.h"

const usint num_of_body_parts = 9;
const usint num_of_body_areas = 12;
const usint num_of_weapon_groups = 5; // max 5 because there are only 5 input events

namespace crusader_part
{
enum body_part { // for indexing arrays
  torso,      // 0
  torso_right,// 1
  torso_left, // 2
  arm_right,  // 3
  arm_left,   // 4
  leg_right,  // 5
  leg_left,   // 6
  leg_right_back,// 7
  leg_left_back// 8
};
};

namespace crusader_area
{
enum body_area { // for indexing arrays
  torso,      // 0
  torso_right,// 1
  torso_left, // 2
  arm_right,  // 3
  arm_left,   // 4
  leg_right,  // 5
  leg_left,   // 6
  torso_back, // 7
  torso_right_back,// 8
  torso_left_back,// 9
  leg_right_back,// 10
  leg_left_back// 11
};
};

enum drive_type {
  drive_type_limbs,
  drive_type_wheels,
  drive_type_tracks,
  drive_type_float,
  drive_type_hover
};

enum drive_subtype {
  drive_subtype_biped,
  drive_subtype_quadruped,
  drive_subtype_multiped,
  drive_subtype_bicycle,
  drive_subtype_tricycle,
  drive_subtype_quad,
  drive_subtype_multi_wheel,
  drive_subtype_unitrack,
  drive_subtype_tracks,
  drive_subtype_multitracks,
  drive_subtype_hovercraft,
  drive_subtype_boat,
  drive_subtype_rocket
};

enum crusader_type {
  crusader_type_recon,
  crusader_type_infantry,
  crusader_type_assault,
  crusader_type_goliath,
  crusader_type_cavarly,
  crusader_type_messanger,
};

// crusader blueprint
struct crusader_design_t {
  string filename;
  string unit;
  string material;
  string hud;
  vector<string> weapons;
  vector<crusader_part::body_part> weapons_placement;
  vector<usint> weapons_extra_ammo;
  vector<internal_type> internals;
  vector<crusader_part::body_part> internals_placement;
  armour_type armour;
  vector<Ogre::Real> armour_placement;
  vector<string> equipment;
  string radar;
  vector<usint> weapon_groups[num_of_weapon_groups];
};

struct crusader_chasis_t {
  crusader_type type;
  string model;
  string name;
  string filename;
  string mesh;
  usint num_of_parts;
  usint num_of_areas;
  Ogre::Real weight; // [t]
  Ogre::Real max_weight; // [t]
  Ogre::Real heat_dissipation_base; // [MJ]
  Ogre::Real heat_dissipation_rate;
  Ogre::Real structure_base;
  vector<usint> internals;
  vector<usint> panels;
  vector<Ogre::Real> surface_area;
  Ogre::Radian torso_arc; // <rad, rad>
  Ogre::Radian arms_arc; // <rad, rad>
  Ogre::Radian torso_turn_speed; // [rad/s]
  Ogre::Radian arms_turn_speed; // [rad/s]
};

struct crusader_drive_t {
  drive_type type;
  drive_subtype subtype;
  Ogre::Real weight; // [t]
  string mesh;
  Ogre::Real kinematic_resistance;
  Ogre::Real kinematic_resistance_reverse;
  Ogre::Radian turn_speed;
  Ogre::Real traction; // - range (0, 1)
  Ogre::Real max_speed; // [m/s] 1 km/h = 0.277777778 m/s
  Ogre::Real max_speed_reverse; // [m/s]
};

struct crusader_engine_t {
  Ogre::Real rating; // [MW]
  Ogre::Real rating_reverse; // [MW]
  Ogre::Real heat; // [MJ]
  Ogre::Real weight; // [t]
  usint size; // [m3]
};

#endif // CRUSADER_DESIGN_H_INCLUDED
