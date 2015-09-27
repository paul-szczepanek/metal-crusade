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
  bool acquireTarget(Unit* a_target);

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
  // called by other object to try and acquire this as a target
  bool acquireAsTarget(Unit* a_targeted_by);
  // called by other objects which hold this as a target to let it know that they no longer do
  void releaseAsTarget(Unit* a_targeted_by);
  // called by targeted object that requires this to relinquish its current target
  bool loseTarget(Unit* a_targeted_by,
                  bool  a_forced = false);

  // return the target of this
  Unit* getTarget();
  // clear targets when this object gets destroyed
  void clearFromTargets();

  // damage reporting
  Real getDamage(usint a_diagram_element);
  void setFormation(Formation* a_formation);
  Formation* getFormation();

protected:
  // targeted objects
  Unit* target;
  // targeted by objects
  vector<Unit*> target_holders;

  Real CoreTemperature;
  Real TotalWeight;
  Vector3 Direction;

  vector<Weapon*> weapons;

  // hud
  bool hud_attached;

  RadarComputer* radar;
  Real CoreIntegrity;
  Real corrected_velocity_scalar;
  GameController* Controller;
  Formation* UnitFormation;
};

Vector3 Unit::getDirection()
{
  return Direction;
}

void Unit::setFormation(Formation* a_formation)
{
  UnitFormation = a_formation;
}

Formation* Unit::getFormation()
{
  return UnitFormation;
}

void Unit::assignController(GameController* a_controller)
{
  Controller = a_controller;
}

// hud operation
Real Unit::getSpeed()
{
  return corrected_velocity_scalar;
}

// return the target of this
Unit* Unit::getTarget()
{
  return target;
}

// orientation
Quaternion Unit::getMovingOrientation()
{
  return Orientation;
}

Quaternion Unit::getLookingOrientation()
{
  return Orientation;
}

// heat
// yeah, I know heat is not the same as temp, this is a simplification
void Unit::addHeat(Real a_heat)
{
  CoreTemperature += a_heat;
}

// hud creation
string Unit::getHudName()
{
  return string("military");
}

void Unit::attachHud(bool a_toggle)
{
  hud_attached = a_toggle;
}

RadarComputer* Unit::getRadar()
{
  return radar;
}

// hud operation
vector<Weapon*>& Unit::getWeapons()
{
  return weapons;
}

Real Unit::getThrottle()
{
  return 0;
}

Real Unit::getCoolant()
{
  return 0;
}

Real Unit::getEngineTemperature()
{
  return 0;
}

Real Unit::getCoreTemperature()
{
  return 0;
}

Real Unit::getPressure()
{
  return CoreIntegrity;
}

#endif // UNIT_H_INCLUDED
