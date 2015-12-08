// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef RADARCOMPUTER_H
#define RADARCOMPUTER_H

#include "main.h"
#include "sphere.h"
#include "game_hud.h"

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
  solid,          // unmoving structures
  solid_target,   // enemies if they are targets and recognised as such
  ally,           // units responding to IFF query
  CurrentTarget,  // enemies if they are targets and recognised as such
  unknown         // everything that doesn't identify as ally
};

struct CorpusDot {
  CorpusDot(Corpus* a_object,
            Real    a_size)
    : object(a_object),
    size(a_size)
  {
  }

  Corpus* object = NULL;
  dot_type type = unknown;
  Real size = 0;
  Vector3 position = Vector3::ZERO;
  bool detected = false;
};

};

struct radar_design_t {
  string filename;
  string model;
  bool active;
  Real power;
  Real heat_sensivity;
  Real electromagnetic_sensivity;
  Real weight;
  radar::sweep_type sweep;
  Radian cone_angle;
  usint heads;
  // strings
  ulint text_name;
  ulint text_description;
  ulint text_list_name;
};

class RadarComputer
{
public:
  RadarComputer(const string& filename,
                Unit*         a_unit);

  void update(const Real a_dt);

  // make active
  void setActive(bool a_toggle);
  bool getActive();

  // toggle radar active of passive
  void setActiveRadar(bool a_toggle);
  bool getActiveRadar();

  // radar range
  Real getRadarRange();
  void setRadarRange(Real a_range);

  vector<radar::CorpusDot>& getDots();

private:
  void updateRadarData();
  // find potential objects to detect
  void updateObjectsWithinRadius();

  bool getRadarDesign(const string&   filename,
                      radar_design_t& radar_design);

private:
  // owner
  Unit* OwnerUnit = NULL;
  // spec
  radar_design_t radar_design;

  // objects within the radar's radius
  vector<radar::CorpusDot> CorpusDots;

  // radar on/off
  bool Enabled = true;
  // radar currently in passive/active mode
  bool ActiveRadar = true;
  Sphere RadarSphere;

  // for checking units only periodically
  Real RefreshInterval = 1;
  Real RefreshAccumulator = 0;
};

inline void RadarComputer::setActive(bool a_toggle)
{
  Enabled = a_toggle;
}

inline vector<radar::CorpusDot>& RadarComputer::getDots()
{
  return CorpusDots;
}

inline bool RadarComputer::getActive()
{
  return Enabled;
}

inline void RadarComputer::setActiveRadar(bool a_toggle)
{
  ActiveRadar = a_toggle;
}

inline bool RadarComputer::getActiveRadar()
{
  return ActiveRadar;
}

inline Real RadarComputer::getRadarRange()
{
  return RadarSphere.Radius;
}

#endif // RADARCOMPUTER_H
