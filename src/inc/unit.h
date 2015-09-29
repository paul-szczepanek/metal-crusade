// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNIT_H_INCLUDED
#define UNIT_H_INCLUDED

#include "corpus.h"
#include "arena_entity.h"

class Weapon;
class RadarComputer;
class ArenaEntity;
class GameController;
class Formation;

class Unit
  : public ArenaEntity
{
public:
  Unit(const string& a_unit_name,
       Vector3       a_pos_xyz,
       Quaternion    a_orientation);
  virtual ~Unit();

  // orientation
  virtual Quaternion getMovingOrientation();
  virtual Quaternion getLookingOrientation();
  virtual Vector3 getDirection();

  // heat
  // yeah, I know heat is not the same as temp, this is a simplification
  void addHeat(Real a_heat);

  // get the angle for firing weapons
  Quaternion getBallisticAngle(const Vector3& a_position);

  // hud creation
  virtual string getHudName();
  void attachHud(bool a_toggle);
  RadarComputer* getRadar();

  // hud operation
  vector<Weapon*>& getWeapons();

  virtual Real getThrottle();
  virtual Real getCoolant();

  virtual vector<usint>& getSelectedGroup() = 0;
  virtual usint getSelectedWeapon() = 0;

  // target acquisition
  void updateTargets();
  bool getUnitAtPointer();

  // partitionening into cells on the arena
  void updateCellIndex();

  virtual Real getEngineTemperature();
  virtual Real getCoreTemperature();
  virtual Real getPressure();

  // hud operation
  Real getSpeed();

  // radar
  bool isDetectable();

  void assignController(GameController* a_controller);

  // targetting
  virtual bool acquireTarget(Corpus* a_target);
  virtual bool acquireAsTargetBy(ArenaEntity* a_entity);
  // called by other objects which hold this as a target to let it know that they no longer do
  virtual void releaseAsTarget(ArenaEntity* a_targeted_by);
  // called by targeted object that requires this to relinquish its current target
  virtual bool loseTarget(ArenaEntity* a_targeted_by,
                  bool         a_forced = false);
  // clear targets when this object gets destroyed
  virtual void clearFromTargets();
  // return the target of this
  virtual ArenaEntity* getTarget();

  // damage reporting
  Real getDamage(usint a_diagram_element);
  void setFormation(Formation* a_formation);
  Formation* getFormation();

protected:
  // targeted objects
  ArenaEntity* target = NULL;
  // targeted by objects
  vector<ArenaEntity*> target_holders;

  Real CoreTemperature = 0;
  Real TotalWeight = 0;
  Vector3 Direction;

  vector<Weapon*> weapons;

  // hud
  bool hud_attached = false;

  //RadarComputer* radar;
  Real CoreIntegrity = 0;
  Real corrected_velocity_scalar = 0;
  GameController* Controller = NULL;
  Formation* UnitFormation = NULL;
};

inline Vector3 Unit::getDirection()
{
  return Direction;
}

inline void Unit::setFormation(Formation* a_formation)
{
  UnitFormation = a_formation;
}

inline Formation* Unit::getFormation()
{
  return UnitFormation;
}

inline void Unit::assignController(GameController* a_controller)
{
  Controller = a_controller;
}

// hud operation
inline Real Unit::getSpeed()
{
  return corrected_velocity_scalar;
}

// return the target of this
inline ArenaEntity* Unit::getTarget()
{
  return target;
}

// orientation
inline Quaternion Unit::getMovingOrientation()
{
  return Orientation;
}

inline Quaternion Unit::getLookingOrientation()
{
  return Orientation;
}

// heat
// yeah, I know heat is not the same as temp, this is a simplification
inline void Unit::addHeat(Real a_heat)
{
  CoreTemperature += a_heat;
}

// hud creation
inline string Unit::getHudName()
{
  return string("military");
}

inline void Unit::attachHud(bool a_toggle)
{
  hud_attached = a_toggle;
}

inline RadarComputer* Unit::getRadar()
{
  //return radar;
}

// hud operation
inline vector<Weapon*>& Unit::getWeapons()
{
  return weapons;
}

inline Real Unit::getThrottle()
{
  return 0;
}

inline Real Unit::getCoolant()
{
  return 0;
}

inline Real Unit::getEngineTemperature()
{
  return 0;
}

inline Real Unit::getCoreTemperature()
{
  return 0;
}

inline Real Unit::getPressure()
{
  return CoreIntegrity;
}

#endif // UNIT_H_INCLUDED
