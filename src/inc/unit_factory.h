// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNIT_FACTORY_H
#define UNIT_FACTORY_H

#include "building_factory.h"

class Unit;
class Crusader;

class UnitFactory
  : public Factory
{
public:
  UnitFactory();
  ~UnitFactory();

  // main loop
  void update(Real a_dt);

  // creation
  Crusader* spawnCrusader(Vector3 a_pos_xyz,
                          const string& a_name,
                          Quaternion a_orientation = Quaternion(1, 0, 0, 0));

  // find unit
  Unit* getUnit(uint a_id);
  Unit* getUnit(const string& a_name);

private:
  list<Unit*> units;
};

#endif // UNIT_FACTORY_H
