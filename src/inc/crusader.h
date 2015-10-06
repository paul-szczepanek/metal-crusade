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
  Crusader(const string&      a_unit_name,
           Vector3            a_pos_xyz,
           Quaternion         a_orientation,
           crusader_design_t  a_design,
           crusader_engine_t  a_engine,
           crusader_drive_t   a_drive,
           crusader_chassis_t a_chassis);
  virtual ~Crusader();

  bool update(const Real a_dt);

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
  void moveCrusader(const Real a_dt);
  void moveTorso(const Real a_dt);
  void updateWeapons(const Real a_dt);
  void pumpHeat(const Real a_dt);
  void shockDamage(const Real a_dt);
  void updateAnimations(const Real a_dt);

  // building the crusader
  void recalculateWeight();
  void extractAnimationStates(Ogre::SceneNode* a_scene_node);
  void positionWeapons();
  void createModel();

  // weapons
  bool fireGroup(usint a_group);
  void cycleGroup();
  void cycleWeapon();

private:
  // moving
  Vector3 Velocity = Vector3::ZERO;
  Vector3 Direction = Vector3::ZERO;
  Vector3 TorsoDirection = Vector3::ZERO;
  Quaternion TorsoOrientation;
  Real Throttle = 0;
  Real TotalWieght = 0;
  Real AngularMomentum = 0;
  Real AngularMomentumTop = 0;
  // distance to feet from main scene node
  Real CrusaderHeight = 0;
  Real DistanceToGround = 0; // from main scene node

  // tracking G forces
  Vector3 ShockDamageOld = Vector3::ZERO;
  Vector3 ShockDamageNew = Vector3::ZERO;

  // design
  crusader_design_t CrusaderDesign;
  crusader_engine_t EngineDesign;
  crusader_drive_t DriveDesign;
  crusader_chassis_t ChassisDesign;

  Corpus* Parts[crusader_corpus::parts_max];

  // integrity and armour
  vector<Real> StructureIntegrity;
  vector<Real> ArmourIntegrity;
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

const Real CRITICAL_TEMPERATURE(500);

inline usint Crusader::getSelectedWeapon()
{
  return WeaponsOperational ? CrusaderDesign.weapon_groups[CurrentGroup][CurrentWeapon] : -1;
}

inline vector<usint>& Crusader::getSelectedGroup()
{
  return CrusaderDesign.weapon_groups[CurrentGroup];
}

inline Real Crusader::getThrottle()
{
  // throttle scaling depends on direction
  return (Throttle > 0) ? Throttle * DriveDesign.max_speed
                        : Throttle * DriveDesign.max_speed_reverse;
}

inline Real Crusader::getDamage(usint a_diagram_element)
{
  return ArmourIntegrity[a_diagram_element] / CrusaderDesign.armour_placement[a_diagram_element];
}

inline Vector3 Crusader::getDirection()
{
  return TorsoDirection;
}

inline Vector3 Crusader::getDriveDirection()
{
  return Direction;
}

inline Quaternion Crusader::getLookingOrientation()
{
  return Orientation * TorsoOrientation;
}

// hud
/*string Crusader::getHudName() {
   return design.hud;
   }*/

inline Real Crusader::getSpeed()
{
  return corrected_velocity_scalar;
}

inline Real Crusader::getCoolant()
{
  return CoolantLevel;
}

inline Real Crusader::getEngineTemperature()
{
  return EngineTemperature;
}

inline Real Crusader::getCoreTemperature()
{
  return CoreTemperature;
}

#endif // CRUSADER_H
