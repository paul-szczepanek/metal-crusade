// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNIT_FACTORY_H
#define UNIT_FACTORY_H

#include "main.h"
#include "building_factory.h"
#include "crusader_design.h"

class Unit;
class Crusader;

class UnitFactory
{
public:
  UnitFactory();
  ~UnitFactory();

  // creation
  Crusader* spawnCrusader(Vector3 a_pos_xyz,
                          const string& a_name,
                          Quaternion a_orientation = Quaternion(1, 0, 0, 0));

  bool getCrusaderSpec(const string&       filename,
                       crusader_engine_t&  engine,
                       crusader_drive_t&   drive,
                       crusader_chassis_t& chassis);
  bool getCrusaderDesign(const string&       filename,
                         crusader_design_t&  design,
                         crusader_engine_t&  engine,
                         crusader_drive_t&   drive,
                         crusader_chassis_t& chassis);

  // find unit
  Unit* getUnit(size_t a_id);
  Unit* getUnit(const string& a_name);

private:
  list<Unit*> Units;
};

#endif // UNIT_FACTORY_H
