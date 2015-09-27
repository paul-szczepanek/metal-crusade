// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CRUSADER_H
#define CRUSADER_H

#include "unit.h"
#include "internal_type.h"
#include "crusader_design.h"
#include "mfd_view_design.h"
namespace crusader_corpus
{

enum parts {
  torso,
  drive,
  arm_r,
  arm_l,
  parts_max,
};

}

class Weapon;
class Animation;
class RadarComputer;
class ParticleEffectStepDust;

class Crusader
  : public Unit
{
public:
  Crusader(const string&     a_unit_name,
           Vector3           a_pos_xyz,
           Quaternion        a_orientation,
           crusader_design_t a_design,
           crusader_engine_t a_engine,
           crusader_drive_t  a_drive,
           crusader_chasis_t a_chasis);
  ~Crusader();

  bool update(Real a_dt);

  bool handleCollision(Collision* a_collision);

  // directions
  Vector3 getDirection();
  Vector3 getDriveDirection();
  Quaternion getLookingOrientation();

  // hud
  //string getHudName();

  Real getSpeed();
  Real getCoolant();
  Real getEngineTemperature();
  Real getCoreTemperature();
  Real getPressure();
  Real getThrottle();

  // damage reporting
  Real getDamage(usint a_diagram_element);
  /*mfd_view::diagram_type getDiagramType() {
     return mfd_view::biped_crusader;
     }*/

  // weapons
  inline vector<usint>& getSelectedGroup();
  inline usint getSelectedWeapon();

private:
  // inner main loop
  void moveCrusader(Real a_dt);
  void moveTorso(Real a_dt);
  void fireWeapons();
  void pumpHeat(Real a_dt);
  void shockDamage(Real a_dt);
  void updateAnimations(Real a_dt);

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

private:
  // moving
  Vector3 Velocity;
  Vector3 Direction;
  Vector3 TorsoDirection;
  Quaternion TorsoOrientation;
  Real Throttle = 0;
  Real TotalWieght = 0;
  Real AngularMomentum = 0;
  Real AngularMomentumTop = 0;
  // distance to feet from main scene node
  Real CrusaderHeight = 0;
  Real DistanceToGround = 0; // from main scene node

  // clamping to the terrain
  Ogre::Ray* terrain_ray = NULL;
  Ogre::RaySceneQuery* terrain_ray_query = NULL;

  // tracking G forces
  Vector3 ShockDamageOld;
  Vector3 ShockDamageNew;

  // design
  crusader_design_t design;
  crusader_engine_t engine;
  crusader_drive_t drive;
  crusader_chasis_t chasis;

  Corpus* Parts[crusader_corpus::parts_max];

  // integrity and armour
  vector<Real> structure;
  vector<Real> armour;
  Real ArmourStructure = 0;
  Real ArmourBallistic = 0;
  Real ArmourConductivity = 0;
  Real ArmourGeneratedHeat = 0;
  // effective heatsinks
  usint Heatsinks = 0;
  // temperature
  Real CoolantLevel = 0;
  Real CoolantQuantity = 0;
  Real EngineTemperature = 0;

  // animation
  Animation* CrusaderAnim = NULL;
  // dust from steps
  ParticleEffectStepDust* StepDust = NULL;

  usint CurrentGroup = 0;
  usint CurrentWeapon = 0;
  bool WeaponsOperational = false;
};

const Real critical_temperature(500);

inline usint Crusader::getSelectedWeapon()
{
  return WeaponsOperational ? design.weapon_groups[CurrentGroup][CurrentWeapon] : -1;
}

inline vector<usint>& Crusader::getSelectedGroup()
{
  return design.weapon_groups[CurrentGroup];
}

inline Real Crusader::getThrottle()
{
  // throttle scaling depends on direction
  return (Throttle > 0) ? Throttle * drive.max_speed : Throttle * drive.max_speed_reverse;
}

inline Real Crusader::getDamage(usint a_diagram_element)
{
  return armour[a_diagram_element] / design.armour_placement[a_diagram_element];
}

Vector3 Crusader::getDirection()
{
  return TorsoDirection;
}

Vector3 Crusader::getDriveDirection()
{
  return Direction;
}

Quaternion Crusader::getLookingOrientation()
{
  return Orientation * TorsoOrientation;
}

// hud
/*string Crusader::getHudName() {
   return design.hud;
   }*/

Real Crusader::getSpeed()
{
  return corrected_velocity_scalar;
}

Real Crusader::getCoolant()
{
  return CoolantLevel;
}

Real Crusader::getEngineTemperature()
{
  return EngineTemperature;
}

Real Crusader::getCoreTemperature()
{
  return CoreTemperature;
}

#endif // CRUSADER_H
