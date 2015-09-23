// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef RADARCOMPUTER_H
#define RADARCOMPUTER_H

#include "main.h"
#include "sphere.h"

class Corpus;
class Corpus;
class Unit;
class GameController;

namespace radar
{
enum sweep_type {
  rotating,
  fixed,
  oscilating
};

enum dot_type {
  solid, // unmoving structures
  solid_target, // enemies if they are targets and recignised as such
  ally, // units responding to iff query
  target, // enemies if they are targets and recignised as such
  unknown // evertyhing that doesn't identify as ally
};

struct CorpusDot {
  CorpusDot(Corpus* a_object, Ogre::Real a_size, Ogre::Vector3 a_position)
    : type(solid), size(a_size), position(a_position), detected(false), object(a_object) { }
  dot_type type;
  Ogre::Real size;
  Ogre::Vector3 position;
  bool detected;
  Corpus* object;
};

struct CorpusDot {
  CorpusDot(Corpus* a_object, Ogre::Real a_size)
    : type(unknown), size(a_size), position(Ogre::Vector3::ZERO), detected(false),
      object(a_object) { }
  dot_type type;
  Ogre::Real size;
  Ogre::Vector3 position;
  bool detected;
  Corpus* object;
};
};

struct radar_design_t {
  string filename;
  string model;
  bool active;
  Ogre::Real power;
  Ogre::Real heat_sensivity;
  Ogre::Real electromagnetic_sensivity;
  Ogre::Real weight;
  radar::sweep_type sweep;
  Ogre::Radian cone_angle;
  usint heads;
  // strings
  ulint text_name;
  ulint text_description;
  ulint text_list_name;
};

class RadarComputer
{
public:
  RadarComputer(const string& filename, Unit* a_unit);
  ~RadarComputer() { };

  // main loop
  void update(Ogre::Real a_dt);

  // make active
  void setActive(bool a_toggle) { active = a_toggle; };
  bool getActive() { return active; };

  // toggle radar active of passive
  void setActiveRadar(bool a_toggle) { active_radar = a_toggle; };
  bool getActiveRadar() { return active_radar; };

  // radar range
  Ogre::Real getRadarRange() { return radar_sphere.radius; };
  void setRadarRange(Ogre::Real a_range);

  // objects within the radar's radius
  vector<radar::CorpusDot> corpus_dots;
  vector<radar::CorpusDot> mobilis_dots;

private:
  // main loop
  void updateRadarData();

  // find potential objects to detect
  void updateObjectsWithinRadius();

  // radar on/off
  bool active;

  // spec
  radar_design_t radar_design;
  // owner
  Unit* unit;

  // radar currently in passive/active mode
  bool active_radar;
  Sphere radar_sphere;

  // for checking units only periodically
  Ogre::Real units_refresh_interval;
  Ogre::Real units_refresh_accumulator;
};

#endif // RADARCOMPUTER_H

