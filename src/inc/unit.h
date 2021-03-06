// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNIT_H_INCLUDED
#define UNIT_H_INCLUDED

#include "corpus.h"
#include "dynamic_entity.h"
#include "mfd_view_design.h"

class Weapon;
class RadarComputer;
class ArenaEntity;
class GameController;
class Formation;

class Unit
  : public DynamicEntity
{
public:
  Unit(const string& a_unit_name);
  virtual ~Unit();

  // orientation
  virtual Vector3 getDriveDirection();
  virtual Quaternion getDriveOrientation();

  // heat
  // yeah, I know heat is not the same as temp, this is a simplification
  void addHeat(Real a_heat);

  // get the angle for firing weapons
  Quaternion getBallisticAngle(const Vector3& a_position);

  // hud creation
  virtual string getHudName();
  void attachHud(bool a_toggle);
  virtual RadarComputer* getRadar();

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
  virtual bool acquireAsTargetBy(Unit* a_entity);
  // called by other objects which hold this as a target to let it know that they no longer do
  virtual void releaseAsTarget(Unit* a_targeted_by);
  // called by targeted object that requires this to relinquish its current target
  virtual bool loseTarget(Unit* a_targeted_by,
                          bool  a_forced = false);
  // clear targets when this object gets destroyed
  virtual void clearFromTargets();
  // return the target of this
  virtual Unit* getTarget();

  // damage reporting
  Real getDamage(usint a_diagram_element);
  void setFormation(Formation* a_formation);
  Formation* getFormation();

  GameController* getController();

  mfd_view::diagram_type getDiagramType();
  Real getSurfaceTemperature();

protected:
  // targeted objects
  Unit* CurrentTarget = NULL;
  // targeted by objects
  vector<Unit*> TargetHolders;

  Real CoreTemperature = 0;
  Real TotalWeight = 0;

  vector<Weapon*> Weapons;

  // hud
  bool HudAttached = false;

  //RadarComputer* radar;
  Real CoreIntegrity = 0;
  Real CorrectedVelocityScalar = 0;
  GameController* Controller = NULL;
  Formation* UnitFormation = NULL;
};

inline Real Unit::getDamage(usint a_diagram_element)
{
  return CoreIntegrity;
}

inline Real Unit::getSurfaceTemperature()
{
  return CoreTemperature;
}

inline mfd_view::diagram_type Unit::getDiagramType()
{
  return mfd_view::object;
}

inline void Unit::setFormation(Formation* a_formation)
{
  UnitFormation = a_formation;
}

inline Formation* Unit::getFormation()
{
  return UnitFormation;
}

inline GameController* Unit::getController()
{
  return Controller;
}

inline void Unit::assignController(GameController* a_controller)
{
  Controller = a_controller;
}

// hud operation
inline Real Unit::getSpeed()
{
  return CorrectedVelocityScalar;
}

// return the target of this
inline Unit* Unit::getTarget()
{
  return CurrentTarget;
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
  HudAttached = a_toggle;
}

inline RadarComputer* Unit::getRadar()
{
  return NULL;
}

// hud operation
inline vector<Weapon*>& Unit::getWeapons()
{
  return Weapons;
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
