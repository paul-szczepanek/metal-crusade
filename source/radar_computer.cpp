//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "radar_computer.h"
#include "files_handler.h"
#include "game.h"
#include "arena.h"
#include "game_controller.h"
#include "unit.h"

const usint num_of_radar_ranges = 7;
const Ogre::Real radar_ranges[num_of_radar_ranges] = { 100, 200, 500, 1000, 2000, 4000, 8000 };

RadarComputer::RadarComputer(const string& filename, Unit* a_unit)
    : active(false), unit(a_unit), active_radar(false), range_index(0),
    units_refresh_interval(1), units_refresh_accumulator(0)
{
    if (FilesHandler::getRadarDesign(filename, radar_design) == false)
        Game::kill(filename+" radar spec garbled! Oh, dear.");

    controller = unit->getController();

    //default to active if radar is capable;
    active_radar = radar_design.active;
}

/** @brief main loop
  */
void RadarComputer::update(Ogre::Real a_dt)
{
    //toggling on and off TODO: add a delay to this, at least to turning on
    if (Game::take(controller->control_block.radar)) {
        active = ~active;
    }

    if (active) {
        // potential units list doesn't need to be updated every frame
        units_refresh_accumulator += a_dt;

        //radar radius change
        if (Game::take(controller->control_block.radar_zoom_in)) {
            //see smaller area
            if (range_index > 0) --range_index;

            //adjust radar sphere
            radar_sphere.radius = radar_ranges[range_index];

        } else if (Game::take(controller->control_block.radar_zoom_out)) {
            //see larger area
            if (++range_index == num_of_radar_ranges) --range_index;

            //adjust radar sphere
            radar_sphere.radius = radar_ranges[range_index];
        }

        if (units_refresh_accumulator > units_refresh_interval) {
            units_refresh_accumulator = 0;
            updateObjectsWithinRadius();
            updateRadarData();
        }

    }

}

/** @brief adjust the data representing the units and buildings on the map
  * TODO: don't refresh all every frame - do a sweep according to radar spec
  */
void RadarComputer::updateRadarData()
{
    list<Unit*>::iterator it = unit_list.begin();
    list<Unit*>::iterator it_end = unit_list.end();

    for (; it != it_end; ++it) {
        //(*it)->getFormation();
    }
}

/** @brief refreshes the list of potential units to be detected by the radar
  * it's very conservative so it doesn't need to be done often.
  * TODO: stagger these updates across units and possibly half the rate for ai
  */
void RadarComputer::updateObjectsWithinRadius()
{
    //adjust the position of the radar sphere
    radar_sphere.centre = unit->getPosition();

    //clear old units
    unit_list.clear();
    //get cell indices within radius
    vector<uint_pair> cell_indices;
    Game::arena->getCellIndicesWithinRadius(unit->getCellIndex(), cell_indices,
                                            radar_sphere.radius);

    //get units within the radius of the radar
    for (usint i = 0, for_size = cell_indices.size(); i < for_size; ++i) {
        list<Corpus*>& unit_corpus_list = Game::arena->getCorpusCell(cell_indices[i]);
        list<Mobilis*>& unit_mobilis_list = Game::arena->getMobilisCell(cell_indices[i]);
        list<Unit*>& unit_cell_list = Game::arena->getUnitCell(cell_indices[i]);

        //if there are any corpus in the cell
        if (unit_corpus_list.size() > 0) {
            //go through them all and check their distance from the radar
            list<Corpus*>::iterator it = unit_corpus_list.begin();
            list<Corpus*>::iterator it_end = unit_corpus_list.end();

            for (; it != it_end; ++it) {
                //if it's within the radar sphere add to the units list for processing
                if (radar_sphere.contains((*it)->getPosition())) {
                    corpus_list.push_back(*it);
                }
            }
        }

        //if there are any mobilis in the cell
        if (unit_mobilis_list.size() > 0) {
            //go through them all and check their distance from the radar
            list<Mobilis*>::iterator it = unit_mobilis_list.begin();
            list<Mobilis*>::iterator it_end = unit_mobilis_list.end();

            for (; it != it_end; ++it) {
                //if it's within the radar sphere add to the units list for processing
                if (radar_sphere.contains((*it)->getPosition())) {
                    mobilis_list.push_back(*it);
                }
            }
        }

        //if there are any units in the cell
        if (unit_cell_list.size() > 0) {
            //go through them all and check their distance from the radar
            list<Unit*>::iterator it = unit_cell_list.begin();
            list<Unit*>::iterator it_end = unit_cell_list.end();

            for (; it != it_end; ++it) {
                //if it's within the radar sphere add to the units list for processing
                if (radar_sphere.contains((*it)->getPosition())) {
                    unit_list.push_back(*it);
                }
            }
        }
    }
}
