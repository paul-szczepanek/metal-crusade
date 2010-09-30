//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "game_controller.h"
#include "formation_factory.h"
#include "game.h"

GameController::GameController(const string& a_name = "Player")
    : turn_speed(0.0), throttle(0.0), pointer_pos(Ogre::Vector3(0, 0, 0)), name(a_name),
    formation(NULL), option_invert_turn_to_pointer(false)
{
    control_block.fire = false;
    control_block.turn_to_pointer = false;
    control_block.crouch = false;
    control_block.align_to_torso = false;
    control_block.fire_group_1 = false;
    control_block.fire_group_2 = false;
    control_block.fire_group_3 = false;
    control_block.fire_group_4 = false;
    control_block.fire_group_5 = false;
    control_block.fire_group_all = false;
    control_block.cycle_weapon = false;
    control_block.cycle_group = false;
    control_block.auto_cycle = true;
    control_block.fire_mode_group = true;
    control_block.inspect = false;
    control_block.nearest_enemy = false;
    control_block.target_low = false;
    control_block.target_high = true;
    control_block.target_air = false;
    control_block.menu_interface = false;
    control_block.lights = false;
    control_block.enemy = false;
    control_block.enemy_back = false;
    control_block.friendly = false;
    control_block.friendly_back = false;
    control_block.target_pointer = false;
    control_block.nav_point = false;
    control_block.nav_point_back = false;
    control_block.power = true;
    control_block.radar = true;
    control_block.flush_coolant = false;
    control_block.nightvision = false;
    control_block.zoom_in = false;
    control_block.zoom_out = false;
    control_block.zoom_target = false;
    control_block.communication_interface = false;
    control_block.radar_zoom_in = false;
    control_block.radar_zoom_out = false;
    control_block.mfd1_select = false;
    control_block.mfd2_select = false;
    control_block.mfd_up = false;
    control_block.mfd_down = false;
    control_block.mfd_left = false;
    control_block.mfd_right = false;
    control_block.mfd_toggle = false;
    control_block.log = false;
};

/** @brief speeds up and slows down
  */
void GameController::setThrottle(Ogre::Real a_throttle)
{
    throttle = a_throttle;

    //cap the throttle to <-1, 1>
    if (throttle > 1) {
        throttle = 1;
    } else if (throttle < -1) {
        throttle = -1;
    }
};

/** @brief sets the formation the player belongs to by name
  */
void GameController::setFormation(string a_formation_name)
{
    setFormation(Game::formation_factory->getFormation(a_formation_name));
};
