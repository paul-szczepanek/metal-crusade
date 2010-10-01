//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "hud_radar_display.h"
#include "radar_computer.h"
#include "game.h"
#include "unit.h"
#include "camera.h"
#include "hud.h"
#include "game_controller.h"

const usint num_of_initial_dots = 16;
const usint num_of_radar_ranges = 4;
const Ogre::Real radar_ranges[num_of_radar_ranges] = { 100, 200, 400, 800 };
const string dot_green_name("hud_radar_dot_green");
const string dot_red_name("hud_radar_dot_red");

HudRadarDisplay::HudRadarDisplay(hud_part_design_t& a_hud_part_design)
    : HudPart(a_hud_part_design), active(false), range_index(0)
{
    //start off with the radar active status read from the radar computer
    active = Game::hud->radar->getActive();
    Game::hud->controller->control_block.radar = active;
    Game::hud->radar->setRadarRange(radar_ranges[range_index]);

    dot_green = Hud::createOverlayMaterial(dot_green_name);
    dot_red = Hud::createOverlayMaterial(dot_red_name);

    for (usint i = 0; i < num_of_initial_dots; ++i) {
        string id = string("radar_dot_")+Game::intIntoString(i);
        dot_elements.push_back(createPanel(id, dot_green_name,
                                           0, 0, 8, 8, container));
        dot_elements.back()->hide();
    }
}

HudRadarDisplay::~HudRadarDisplay()
{

}

void HudRadarDisplay::update(Ogre::Real a_dt)
{
    //toggling on and off TODO: add a delay to this, at least to turning on
    if (active != Game::hud->controller->control_block.radar) {
        active = !active;
        Game::hud->radar->setActive(active);
        if (!active) {
            for (uint i = 0, for_size = dot_elements.size(); i < for_size; ++i) {
                dot_elements[i]->hide();
            }
        }
    }

    if(active) {
        //radar radius change
        if (Game::take(Game::hud->controller->control_block.radar_zoom_in)) {
            //see smaller area
            if (range_index > 0) --range_index;

            //adjust radar sphere
            Game::hud->radar->setRadarRange(radar_ranges[range_index]);

        } else if (Game::take(Game::hud->controller->control_block.radar_zoom_out)) {
            //see larger area
            if (++range_index == num_of_radar_ranges) --range_index;

            //adjust radar sphere
            Game::hud->radar->setRadarRange(radar_ranges[range_index]);
        }

        //how many dots on the screen?
        vector<radar::MobilisDot>& dots = Game::hud->radar->mobilis_dots;
        uint num_of_dots = dots.size();

        //grow number of dots if needed
        if (dot_elements.size() < num_of_dots) {
            for (uint i = 0, for_size = num_of_dots - dot_elements.size(); i < for_size; ++i) {
                string id = string("radar_dot_")+Game::intIntoString(dot_elements.size());
                dot_elements.push_back(createPanel(id, dot_green_name,
                                                   0, 0, 8, 8, container));
                dot_elements.back()->hide();
            }
        }

        Ogre::Real radar_angle = (Game::camera->getOrientation().getYaw()).valueRadians();

        Ogre::Real x0 = Game::hud->player_unit->getX();
        Ogre::Real y0 = Game::hud->player_unit->getZ();
        Ogre::Real range_scale = size.first / Game::hud->radar->getRadarRange();

        for (uint i = 0, for_size = dot_elements.size(); i < for_size; ++i) {
            if (i < num_of_dots && dots[i].detected) {
                Ogre::Real dot_size = max(dots[i].size * range_scale, Ogre::Real(8));
                dot_elements[i]->setDimensions(dot_size, dot_size);
                Ogre::Real x = (dots[i].position.x - x0) * range_scale;
                Ogre::Real y = (dots[i].position.z - y0) * range_scale;

                x = (cos(radar_angle) * x - sin(radar_angle) * y) * 0.5
                    + (size.first - dot_size) * 0.5;
                y = (sin(radar_angle) * x + cos(radar_angle) * y) * 0.5
                    + (size.first - dot_size) * 0.5;

                dot_elements[i]->setPosition(x, y);
                dot_elements[i]->show();
            } else {
                dot_elements[i]->hide();
            }
        }
    }
}
