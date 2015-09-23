// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNIT_FACTORY_H
#define UNIT_FACTORY_H

#include "building_factory.h"

class Unit;
class Crusader;

class UnitFactory : public BuildingFactory
{
public:
  UnitFactory();
  ~UnitFactory();

  // main loop
  void update(Ogre::Real a_dt);

  // creation
  Crusader* spawnCrusader(Ogre::Vector3 a_pos_xyz, const string& a_name,
                          Ogre::Quaternion a_orientation = Ogre::Quaternion(1, 0, 0, 0));

  // find unit
  Unit* getUnit(uint a_id);
  Unit* getUnit(const string& a_name);

  list<Unit*>::iterator getUnitIteratorBegin() { return units.begin(); };
  list<Unit*>::iterator getUnitIteratorEnd() { return units.end(); };

private:
  list<Unit*> units;
};

#endif // UNIT_FACTORY_H
