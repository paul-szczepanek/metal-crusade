//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "radar_computer.h"
#include "files_handler.h"
#include "game.h"
#include "arena.h"
#include "game_controller.h"
#include "unit.h"

const Ogre::Real bs_to_dot_size = 1 / root_of_2;

RadarComputer::RadarComputer(const string& filename, Unit* a_unit)
    : active(true), unit(a_unit), active_radar(false), units_refresh_interval(1),
    units_refresh_accumulator(0)
{
    if (FilesHandler::getRadarDesign(filename, radar_design) == false)
        Game::kill(filename+" radar spec garbled! Oh, dear.");

    //default to active if radar is capable;
    active_radar = radar_design.active;
}

/** @brief main loop
  */
void RadarComputer::update(Ogre::Real a_dt)
{
    if (active) {
        // potential units list doesn't need to be updated every frame
        units_refresh_accumulator += a_dt;

        if (units_refresh_accumulator > units_refresh_interval) {
            units_refresh_accumulator = 0;
            updateObjectsWithinRadius();
        }
        updateRadarData();
    }
}

void RadarComputer::setRadarRange(Ogre::Real a_range)
{
    radar_sphere.radius = a_range;
    updateObjectsWithinRadius();
}

/** @brief adjust the data representing the units and buildings on the map
  * TODO: don't refresh all every frame - do a sweep according to radar spec
  * TODO: detection mechanic - range, power, heat, electronics, ray not obstructed etc.
  * TODO: obviously simplify mechanic for ais
  */
void RadarComputer::updateRadarData()
{
    //do the units
    vector<radar::MobilisDot>::iterator it = mobilis_dots.begin();
    vector<radar::MobilisDot>::iterator it_end = mobilis_dots.end();

    for (; it != it_end; ++it) {
        (*it).position = (*it).object->getPosition();
        (*it).detected = true;
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
    //adjust the position of the radar sphere
    radar_sphere.centre = unit->getPosition();

    //clear old objects
    corpus_dots.clear();
    mobilis_dots.clear();

    //get cell indices within radius
    vector<uint_pair> cell_indices;
    Game::arena->getCellIndicesWithinRadius(unit->getCellIndex(), cell_indices,
                                            radar_sphere.radius);

    //for cells within the radius of the radar
    for (uint i = 0, for_size = cell_indices.size(); i < for_size; ++i) {
        //get objects' lists
        list<Corpus*>& unit_corpus_list = Game::arena->getCorpusCell(cell_indices[i]);
        list<Mobilis*>& unit_mobilis_list = Game::arena->getMobilisCell(cell_indices[i]);
        list<Unit*>& unit_cell_list = Game::arena->getUnitCell(cell_indices[i]);

        //if there are any corpus in the cell
        if (unit_corpus_list.size() > 0) {
            //go through them all and check their distance from the radar
            list<Corpus*>::iterator it = unit_corpus_list.begin();
            list<Corpus*>::iterator it_end = unit_corpus_list.end();

            for (; it != it_end; ++it) {
                //if it's within the radar sphere add to the list for processing
                if (radar_sphere.contains((*it)->getPosition())) {
                    Ogre::Real size = (*it)->getBoundingSphere().radius * bs_to_dot_size;
                    corpus_dots.push_back(radar::CorpusDot((*it), size, (*it)->getPosition()));
                }
            }
        }

        //if there are any mobilis in the cell
        if (unit_mobilis_list.size() > 0) {
            //go through them all and check their distance from the radar
            list<Mobilis*>::iterator it = unit_mobilis_list.begin();
            list<Mobilis*>::iterator it_end = unit_mobilis_list.end();

            for (; it != it_end; ++it) {
                //if it's within the radar sphere add to the list for processing
                if (radar_sphere.contains((*it)->getPosition())) {
                    //get rid of tiny projectiles and things you're not able to target
                    if ((*it)->isDetectable()) {
                        Ogre::Real size = (*it)->getBoundingSphere().radius * bs_to_dot_size;
                        mobilis_dots.push_back(radar::MobilisDot((*it), size));
                    }
                }
            }
        }

        //if there are any units in the cell
        if (unit_cell_list.size() > 0) {
            //go through them all and check their distance from the radar
            list<Unit*>::iterator it = unit_cell_list.begin();
            list<Unit*>::iterator it_end = unit_cell_list.end();

            for (; it != it_end; ++it) {
                //if it's within the radar sphere add to the list for processing
                if (radar_sphere.contains((*it)->getPosition())) {
                    Ogre::Real size = (*it)->getBoundingSphere().radius * bs_to_dot_size;
                    mobilis_dots.push_back(radar::MobilisDot((*it), size));
                }
            }
        }
    }
}
