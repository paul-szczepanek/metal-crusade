// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "radar_computer.h"
#include "files_handler.h"
#include "game.h"
#include "game_arena.h"
#include "game_controller.h"
#include "unit.h"
#include "text_store.h"

#define BS_TO_DOT_SIZE (1 / root_of_2)

RadarComputer::RadarComputer(const string& filename,
                             Unit*         a_unit)
  : OwnerUnit(a_unit)
{
  if (getRadarDesign(filename, radar_design) == false) {
    Game::kill(filename + " radar spec garbled! Oh, dear.");
  }

  // default to active if radar is capable
  ActiveRadar = radar_design.active;
}

/** @brief load weapon spec from file
 */
bool RadarComputer::getRadarDesign(const string&   filename,
                                   radar_design_t& radar_design)
{
  using namespace FilesHandler;
  // prepare map to read data into
  map<string, string> pairs;
  //insert data from file into pairs
  assert(getPairs(filename, RADAR_DIR, pairs));

  // fill structs with info from pairs
  radar_design.filename = filename;
  radar_design.model = pairs["radar_design.model"];
  // game text
  radar_design.text_name = Game::Text->getStringKey(pairs["radar_design.text_name"]);
  radar_design.text_list_name = Game::Text->getStringKey(pairs["radar_design.text_list_name"]);
  radar_design.text_description = Game::Text->getStringKey(pairs["radar_design.text_description"]);
  // basic properties
  radar_design.weight = getReal(pairs["radar_design.weight"]);
  // radar type
  radar_design.active = getBool(pairs["radar_design.active"]);
  radar_design.sweep = radar::sweep_type(getEnum(pairs["radar_design.sweep"]));
  radar_design.cone_angle = Ogre::Radian(getReal(pairs["radar_design.cone_angle"]) * degree2rad);
  radar_design.heads = intoInt(pairs["radar_design.heads"]);
  // radar parameters
  radar_design.power = getReal(pairs["radar_design.power"]);
  radar_design.heat_sensivity = getReal(pairs["radar_design.power"]);
  radar_design.electromagnetic_sensivity =
    getReal(pairs["radar_design.electromagnetic_sensivity"]);

  return true;
}

/** @brief main loop
 */
void RadarComputer::update(Real a_dt)
{
  // toggling on and off TODO: add a delay to this, at least to turning on
  if (Enabled != Game::Hud->Controller->ControlBlock.radar) {
    Enabled = !Enabled;
  }

  if (Enabled) {
    // potential units list doesn't need to be updated every frame
    RefreshAccumulator += a_dt;

    if (RefreshAccumulator > RefreshInterval) {
      RefreshAccumulator = 0;
      updateObjectsWithinRadius();
    }
    updateRadarData();
  }
}

void RadarComputer::setRadarRange(Real a_range)
{
  RadarSphere.Radius = a_range;
  updateObjectsWithinRadius();
}

/** @brief adjust the data representing the units and buildings on the map
 * TODO: don't refresh all every frame - do a sweep according to radar spec
 * TODO: detection mechanic - range, power, heat, electronics, ray not obstructed etc.
 * TODO: obviously simplify mechanic for ais
 */
void RadarComputer::updateRadarData()
{
  for (radar::CorpusDot& dot : CorpusDots) {
    dot.position = dot.object->XYZ;
    dot.detected = true;
  }
}

/** @brief refreshes the list of potential units to be detected by the radar
 * it's very conservative so it doesn't need to be done often.
 * TODO: stagger these updates across units and possibly half the rate for ai
 * TODO: sort them according to radar type so that you can access them
 * in sequence when doing the sweep
 */
void RadarComputer::updateObjectsWithinRadius()
{
  // adjust the position of the radar sphere
  RadarSphere.Centre = OwnerUnit->getXYZ();

  // clear old objects
  CorpusDots.clear();

  // get cell indexes within radius
  vector<size_t_pair> cell_indexes;
  Game::Arena->getCellIndexesWithinRadius(OwnerUnit->getXYZ(), cell_indexes,
                                          RadarSphere.Radius);

  // for cells within the radius of the radar
  for (size_t_pair index : cell_indexes) {
    // get objects' lists
    list<Corpus*>& unit_corpus_list = Game::Arena->getCorpusCell(index);

    // if there are any corpus in the cell
    if (unit_corpus_list.size() > 0) {
      for (Corpus* c : unit_corpus_list) {
        // if it's within the radar sphere add to the list for processing
        if (RadarSphere.contains(c->XYZ)) {
          Real dot_size = c->BoundingSphere.Radius * BS_TO_DOT_SIZE;
          CorpusDots.push_back(radar::CorpusDot(c, dot_size));
        }
      }
    }
  }
}
