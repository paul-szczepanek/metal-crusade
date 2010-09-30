//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "radar_computer.h"
#include "game.h"
#include "hud.h"
#include "game_controller.h"

RadarComputer::RadarComputer()
    : active(false), active_radar(false), range(0)
{

}

/** @brief main loop because radar needs input
  */
void RadarComputer::update(Ogre::Real a_dt)
{
    //toggling on and off TODO: add a delay to this, at least to turning on
    if (Game::take(Game::hud->controller->control_block.radar)) {
        active = ~active;
    }

    if (active) {

    }
}

