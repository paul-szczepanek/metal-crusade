// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNIT_H_INCLUDED
#define UNIT_H_INCLUDED

#include "mobilis.h"

class Weapon;
class RadarComputer;

class Unit : public Corpus
{
public:
  Unit(Ogre::Vector3 a_pos_xyz, const string& a_unit_name, Ogre::SceneNode* a_scene_node,
       Ogre::Quaternion a_orientation);
  virtual ~Unit() { };

  // orientation
  virtual Ogre::Quaternion getMovingOrientation() { return orientation; };
  virtual Ogre::Quaternion getLookingOrientation() { return orientation; };

  // heat
  // yeah, I know heat is not the same as temp, this is a simplifaication
  void addHeat(Ogre::Real a_heat) { core_temperature += a_heat; };

  // get the angle for firing weapons
  Ogre::Quaternion getBallisticAngle(const Ogre::Vector3& a_position);

  // hud creation
  virtual string getHudName() { return string("military"); };
  void attachHud(bool a_toggle) { hud_attached = a_toggle; };
  RadarComputer* getRadar() { return radar; };

  // hud operation
  vector<Weapon*>& getWeapons() { return weapons; };
  virtual Ogre::Real getThrottle() { return 0; };
  virtual Ogre::Real getCoolant()  { return 0; };
  virtual vector<usint>& getSelectedGroup() = 0;
  virtual usint getSelectedWeapon() = 0;

  // target acquisition
  void updateTargets();
  bool getUnitAtPointer();
  bool acquireTarget(Corpus* a_target);

  // partitionening into cells on the arena
  void updateCellIndex();

protected:
  Ogre::Real core_temperature;

  vector<Weapon*> weapons;

  // hud
  bool hud_attached;

  RadarComputer* radar;
};

#endif // UNIT_H_INCLUDED
