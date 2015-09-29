// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "unit_factory.h"
#include "files_handler.h"
#include "query_mask.h"
#include "game.h"
#include "crusader.h"
#include "unit.h"

UnitFactory::UnitFactory()
{
}

/** @brief destroys all units and ogre objects
 */
UnitFactory::~UnitFactory()
{
  for (auto u : Units) {
    delete u;
  }
}

Unit* UnitFactory::getUnit(size_t a_id)
{
  // TEMP!!!!!!!!
  list<Unit*>::iterator it_end = Units.end();
  for (list<Unit*>::iterator it = Units.begin(); it != it_end; ) {
    return *it;
  }
  return *it_end;
}

/** @brief creates units and add's them to a list
 */
Crusader* UnitFactory::spawnCrusader(Vector3       a_pos_xyz,
                                     const string& a_name,
                                     Quaternion    a_orientation)
{
  // structs for spec for crusader
  crusader_engine_t engine;
  crusader_drive_t drive;
  crusader_chassis_t chassis;
  crusader_design_t design;

  // load spec from file
  getCrusaderDesign(a_name, design, engine, drive, chassis);

  // get unique string from id
  string id_string = Game::getUniqueID() + chassis.mesh; // append unique id to name

  // pass the root node (drive) to the crusader object
  Crusader* spawn = new Crusader(id_string, a_pos_xyz, a_orientation,
                                 design, engine, drive, chassis);

  // put the unit on the list
  Units.push_back(spawn);

  return spawn;
}

/** @brief load the customisable design of a crusader
 * fills struct with design read from a file
 */
bool UnitFactory::getCrusaderDesign(const string&       filename,
                                    crusader_design_t&  design,
                                    crusader_engine_t&  engine,
                                    crusader_drive_t&   drive,
                                    crusader_chassis_t& chassis)
{
  using namespace crusader_part;
  using namespace FilesHandler;

  //prepare map to read data into
  map<string, string> pairs;
  //insert data from file into pairs
  assert(getPairs(filename, UNIT_DIR, pairs));

  //fill structs with info from pairs
  design.filename = filename;
  design.unit = pairs["design.unit"];

  //load spec
  getCrusaderSpec(design.unit, engine, drive, chassis);
  getStringArray(design.weapons, pairs["design.weapons"]);
  vector<Ogre::Real> default_0(chassis.num_of_areas, 0);   //defaults 0 armour
  design.armour_placement = default_0;

  //continue loading the design
  design.material = pairs["design.material"];
  design.hud = pairs["design.hud"];
  design.radar = pairs["design.radar"];
  getUsintArray(design.weapons_extra_ammo, pairs["design.weapons_extra_ammo"]);
  getEnumArray(design.weapons_placement, pairs["design.weapons_placement"]);
  getStringArray(design.equipment, pairs["design.equipment"]);
  getEnumArray(design.internals, pairs["design.internals"]);
  getEnumArray(design.internals_placement, pairs["design.internals_placement"]);
  design.armour =
    armour_type(getEnum(pairs["design.armour"]));

  //armour placement
  design.armour_placement[crusader_area::torso] =
    getReal(pairs["design.armour_placement.torso"]);
  design.armour_placement[crusader_area::torso_right] =
    getReal(pairs["design.armour_placement.torso_right"]);
  design.armour_placement[crusader_area::torso_left] =
    getReal(pairs["design.armour_placement.torso_left"]);
  design.armour_placement[crusader_area::arm_right] =
    getReal(pairs["design.armour_placement.arm_right"]);
  design.armour_placement[crusader_area::arm_left] =
    getReal(pairs["design.armour_placement.arm_left"]);
  design.armour_placement[crusader_area::leg_right] =
    getReal(pairs["design.armour_placement.leg_right"]);
  design.armour_placement[crusader_area::leg_left] =
    getReal(pairs["design.armour_placement.leg_left"]);
  design.armour_placement[crusader_area::torso_back] =
    getReal(pairs["design.armour_placement.torso_back"]);
  design.armour_placement[crusader_area::torso_right_back] =
    getReal(pairs["design.armour_placement.torso_right_back"]);
  design.armour_placement[crusader_area::torso_left_back] =
    getReal(pairs["design.armour_placement.torso_left_back"]);

  //read weapons into weapon groups
  for (size_t i = 0; i < num_of_weapon_groups; ++i) {
    stringstream i_stream;
    i_stream << "design.weapon_groups." << i;
    getUsintArray(design.weapon_groups[i], pairs[i_stream.str()]);
  }

  return true;
}

/** @brief load inmutable specification of a crusader linked to the graphics
 * fills structs with specs read from a file
 */
bool UnitFactory::getCrusaderSpec(const string&       filename,
                                  crusader_engine_t&  engine,
                                  crusader_drive_t&   drive,
                                  crusader_chassis_t& chassis)
{
  using namespace crusader_part;
  using namespace FilesHandler;

  //prepare map to read data into
  map<string, string> pairs;
  //insert data from file into pairs
  assert(getPairs(filename, CRUSADER_DIR, pairs));

  //fill structs with info from pairs
  chassis.filename = filename;
  chassis.name = pairs["chassis.name"];
  chassis.model = pairs["chassis.model"];
  chassis.type = crusader_type(getEnum(pairs["chassis.type"]));
  engine.rating = getReal(pairs["engine.rating"]);   //effective[KN]
  engine.rating_reverse = getReal(pairs["engine.rating_reverse"]);   //effective[KN]
  engine.heat = getReal(pairs["engine.heat"]);   //[MJ]
  engine.weight = getReal(pairs["engine.weight"]);   //[t]
  engine.size = intoInt(pairs["engine.size"]);   //[m3]
  drive.type = drive_type(getEnum(pairs["drive.type"]));
  drive.subtype = drive_subtype(getEnum(pairs["drive.subtype"]));
  drive.mesh = pairs["drive.mesh"];
  drive.max_speed = getReal(pairs["drive.max_speed"]);   //[m/s]
  drive.max_speed_reverse = getReal(pairs["drive.max_speed_reverse"]);   //[m/s]
  drive.weight = getReal(pairs["drive.weight"]);   //[t]
  drive.turn_speed = getReal(pairs["drive.turn_speed"]) * pi;   //[rad/s]
  drive.kinematic_resistance = getReal(pairs["drive.kinematic_resistance"]);   //[KN]
  drive.kinematic_resistance_reverse = getReal(pairs["drive.kinematic_resistance_reverse"]);
  drive.traction = getReal(pairs["drive.traction"]);   //- range (0, 1)

  //TODO: add more types of drives an read more parts
  if (drive.subtype == drive_subtype_multiped) {
  chassis.num_of_parts = 9;
  chassis.num_of_areas = 12;

  } else {
    //default for most
    chassis.num_of_parts = 7;
    chassis.num_of_areas = 10;
  }

  chassis.internals.reserve(chassis.num_of_parts);
  chassis.panels.reserve(chassis.num_of_parts);
  chassis.surface_area.reserve(chassis.num_of_areas);

  //defaults all to 0
  vector<Ogre::Real> default_0_real(chassis.num_of_areas, 0);
  chassis.surface_area = default_0_real;
  vector<usint> defualt_0(chassis.num_of_parts, 0);
  chassis.internals = defualt_0;
  chassis.panels = defualt_0;

  //continue loading
  chassis.mesh = pairs["chassis.mesh"];
  chassis.weight = getReal(pairs["chassis.weight"]);   //[t]
  chassis.max_weight = getReal(pairs["chassis.max_weight"]);   //[t]
  chassis.heat_dissipation_base = getReal(pairs["chassis.heat_dissipation_base"]);   //[MJ]
  chassis.heat_dissipation_rate = getReal(pairs["chassis.heat_dissipation_rate"]);   //multpiplier
  chassis.torso_arc = getReal(pairs["chassis.torso_arc"]) * pi;   //[rad]
  chassis.arms_arc = getReal(pairs["chassis.arms_arc"]) * pi;   //[rad]
  chassis.torso_turn_speed = getReal(pairs["chassis.torso_turn_speed"]) * pi;   //[rad/s]
  chassis.arms_turn_speed = getReal(pairs["chassis.arms_turn_speed"]) * pi;   //[rad/s]
  chassis.structure_base = getReal(pairs["chassis.structure_base"]);   //[rad/s]
  chassis.internals[torso] = intoInt(pairs["chassis.internals.torso"]);   //[m3]
  chassis.internals[torso_right] = intoInt(pairs["chassis.internals.torso_right"]);
  chassis.internals[torso_left] = intoInt(pairs["chassis.internals.torso_left"]);
  chassis.internals[arm_left] = intoInt(pairs["chassis.internals.arm_left"]);
  chassis.internals[arm_right] = intoInt(pairs["chassis.internals.arm_right"]);
  chassis.internals[leg_left] = intoInt(pairs["chassis.internals.leg_left"]);
  chassis.internals[leg_right] = intoInt(pairs["chassis.internals.leg_right"]);
  chassis.panels[torso] = intoInt(pairs["chassis.panels.torso"]);
  chassis.panels[torso_right] = intoInt(pairs["chassis.panels.torso_right"]);
  chassis.panels[torso_left] = intoInt(pairs["chassis.panels.torso_left"]);
  chassis.panels[arm_right] = intoInt(pairs["chassis.panels.arm_right"]);
  chassis.panels[arm_left] = intoInt(pairs["chassis.panels.arm_left"]);
  chassis.panels[leg_right] = intoInt(pairs["chassis.panels.leg_right"]);
  chassis.panels[leg_left] = intoInt(pairs["chassis.panels.leg_left"]);
  chassis.surface_area[crusader_area::torso] =
    getReal(pairs["chassis.surface_area.torso"]);
  chassis.surface_area[crusader_area::torso_right] =
    getReal(pairs["chassis.surface_area.torso_right"]);
  chassis.surface_area[crusader_area::torso_left] =
    getReal(pairs["chassis.surface_area.torso_left"]);
  chassis.surface_area[crusader_area::arm_left] =
    getReal(pairs["chassis.surface_area.arm_left"]);
  chassis.surface_area[crusader_area::arm_right] =
    getReal(pairs["chassis.surface_area.arm_right"]);
  chassis.surface_area[crusader_area::leg_left] =
    getReal(pairs["chassis.surface_area.leg_left"]);
  chassis.surface_area[crusader_area::leg_right] =
    getReal(pairs["chassis.surface_area.leg_right"]);
  chassis.surface_area[crusader_area::torso_back] =
    getReal(pairs["chassis.surface_area.torso_back"]);
  chassis.surface_area[crusader_area::torso_right_back] =
    getReal(pairs["chassis.surface_area.torso_right_back"]);
  chassis.surface_area[crusader_area::torso_left_back] =
    getReal(pairs["chassis.surface_area.torso_left_back"]);

  return true;
}
