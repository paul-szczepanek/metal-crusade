// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNIT_H_INCLUDED
#define UNIT_H_INCLUDED

#include "corpus.h"

class Weapon;
class RadarComputer;

class Unit
{
public:
  Unit(Vector3          a_pos_xyz,
       const string&    a_unit_name,
       Ogre::SceneNode* a_scene_node,
       Quaternion       a_orientation);
  virtual ~Unit() {
  }

  // orientation
  virtual Quaternion getMovingOrientation() {
    return orientation;
  }

  virtual Quaternion getLookingOrientation() {
    return orientation;
  }

  // heat
  // yeah, I know heat is not the same as temp, this is a simplifaication
  void addHeat(Real a_heat) {
    core_temperature += a_heat;
  }

  // get the angle for firing weapons
  Quaternion getBallisticAngle(const Vector3& a_position);

  // hud creation
  virtual string getHudName() {
    return string("military");
  }

  void attachHud(bool a_toggle) {
    hud_attached = a_toggle;
  }

  RadarComputer* getRadar() {
    return radar;
  }

  // hud operation
  vector<Weapon*>& getWeapons() {
    return weapons;
  }

  virtual Real getThrottle() {
    return 0;
  }

  virtual Real getCoolant()  {
    return 0;
  }

  virtual vector<usint>& getSelectedGroup() = 0;
  virtual usint getSelectedWeapon() = 0;

  // target acquisition
  void updateTargets();
  bool getUnitAtPointer();
  bool acquireTarget(Corpus* a_target);

  // partitionening into cells on the arena
  void updateCellIndex();

  virtual Real getEngineTemperature()  {
    return 0;
  }

  virtual Real getCoreTemperature()  {
    return 0;
  }

  virtual Real getPressure()  {
    return core_integrity;
  }

  // hud operation
  Real getSpeed();

  // targetting
  // called by other object to try and aqcuire this as a target
  bool acquireAsTarget(Corpus* a_targeted_by);
  // called by other objects which hold this as a target to let it know that they no longer do
  void releaseAsTarget(Corpus* a_targeted_by);
  // called by targeted object that requires this to relinquish its current target
  bool loseTarget(Corpus* a_targeted_by,
                  bool    a_forced = false);

  // radar
  bool isDetectable();

  // return the target of this
  Corpus* getTarget();

  // clear targets when this object gets destroyed
  void clearFromTargets();

  // damage reporting
  Real getDamage(usint a_diagram_element);

protected:
  // targeted objects
  Corpus* target;
  // targeted by objects
  vector<Corpus*> target_holders;

  Real core_temperature;
  Quaternion orientation;

  vector<Weapon*> weapons;

  // hud
  bool hud_attached;

  RadarComputer* radar;
};

// return the target of this
Unit* Unit::getTarget() {
  return target;
}

#endif // UNIT_H_INCLUDED
