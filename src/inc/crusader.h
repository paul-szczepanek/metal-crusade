// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CRUSADER_H
#define CRUSADER_H

#include "unit.h"
#include "internal_type.h"
#include "crusader_design.h"
#include "mfd_view_design.h"

class Weapon;
class Animation;
class RadarComputer;
class ParticleEffectStepDust;

class Crusader
  : public Unit
{
public:
  Crusader(Vector3           a_pos_xyz,
           const string&     a_unit_name,
           Ogre::SceneNode*  a_scene_node,
           Quaternion        a_orientation,
           crusader_design_t a_design,
           crusader_engine_t a_engine,
           crusader_drive_t  a_drive,
           crusader_chasis_t a_chasis);
  ~Crusader();

  // main loop
  int update();

  // collision
  int handleCollision(Collision* a_collision);

  // directions
  Vector3 getDirection() {
    return torso_direction;
  }

  Vector3 getDriveDirection() {
    return direction;
  }

  Quaternion getLookingOrientation() {
    return orientation * torso_orientation;
  }

  // ramming damage
  Real getBallisticDmg();

  // hud
  string getHudName() {
    return design.hud;
  }

  Real getSpeed() {
    return corrected_velocity_scalar;
  }

  Real getCoolant() {
    return coolant_level;
  }

  Real getEngineTemperature() {
    return engine_temperature;
  }

  Real getCoreTemperature() {
    return core_temperature;
  }

  Real getSurfaceTemperature() {
    return surface_temperature;
  }

  Real getPressure() {
    return core_integrity;
  }

  Real getThrottle();

  // damage reporting
  Real getDamage(usint a_diagram_element);
  mfd_view::diagram_type getDiagramType() {
    return mfd_view::biped_crusader;
  }

  // weapons
  inline vector<usint>& getSelectedGroup();
  inline usint getSelectedWeapon();

private:
  // inner main loop
  void moveCrusader();
  void moveTorso();
  void fireWeapons();
  void pumpHeat();
  void shockDamage();
  void updateAnimations();

  // helper functions
  void localiseAngle(Radian&       angle,
                     const Radian& global_angle);

  // building the crusader
  void recalculateWeight();
  void extractAnimationStates(Ogre::SceneNode* a_scene_node);
  void positionWeapons(vector<Vector3>& panel_positions,
                       vector<usint>&   slots_used);

  // weapons
  bool fireGroup(usint a_group);
  void cycleGroup();
  void cycleWeapon();
  bool weapons_operational;

  usint current_group;
  usint current_weapon;

  // tracking G forces
  Vector3 shock_damage_old;
  Vector3 shock_damage_new;

  // design
  crusader_design_t design;
  crusader_engine_t engine;
  crusader_drive_t drive;
  crusader_chasis_t chasis;

  // effective heatsinks
  usint heatsinks;

  // node that rotates the torso
  Ogre::SceneNode* torso_node;

  // moving
  Real throttle;

  // dust from steps
  ParticleEffectStepDust* step_dust;

  // orientation
  int angular_momentum_top;
  Quaternion torso_orientation;
  Vector3 torso_direction;

  // distance to ground from main scene node
  Real crusader_height;

  // clamping to the terrain
  Ogre::Ray* terrain_ray;
  Ogre::RaySceneQuery* terrain_ray_query;

  // integrity and armour
  vector<Real> structure;
  vector<Real> armour;
  Real armour_structure;
  Real armour_ballistic;
  Real armour_conductivity;
  Real armour_generated_heat;

  // temperature
  Real coolant_level;
  Real coolant;
  Real engine_temperature;

  // animation
  Animation* animation;
};

const Real critical_temperature(500);

inline usint Crusader::getSelectedWeapon()
{
  return weapons_operational ? design.weapon_groups[current_group][current_weapon] : -1;
}

inline vector<usint>& Crusader::getSelectedGroup()
{
  return design.weapon_groups[current_group];
}

inline Real Crusader::getBallisticDmg()
{
  Real dmg = log10(velocity.length());
  if (dmg > 0) {
    return dmg * penetration;
  } else {
    return 0;
  }
}

inline Real Crusader::getThrottle()
{
  // throttle scaling depends on direction
  return (throttle > 0) ? throttle * drive.max_speed : throttle * drive.max_speed_reverse;
}

inline Real Crusader::getDamage(usint a_diagram_element)
{
  return armour[a_diagram_element] / design.armour_placement[a_diagram_element];
}

#endif // CRUSADER_H
