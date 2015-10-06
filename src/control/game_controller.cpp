// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game_controller.h"
#include "formation_manager.h"
#include "game.h"

GameController::~GameController()
{
}

GameController::GameController(const string& a_name = "Player")
  : TurnSpeed(0.0),
  Throttle(0.0),
  PointerPos(Vector3(0, 0, 0)),
  Name(a_name),
  InvertTurnToPointer(false)
{
  ControlBlock.fire = false;
  ControlBlock.turn_to_pointer = false;
  ControlBlock.crouch = false;
  ControlBlock.align_to_torso = false;
  ControlBlock.fire_group_1 = false;
  ControlBlock.fire_group_2 = false;
  ControlBlock.fire_group_3 = false;
  ControlBlock.fire_group_4 = false;
  ControlBlock.fire_group_5 = false;
  ControlBlock.fire_group_all = false;
  ControlBlock.cycle_weapon = false;
  ControlBlock.cycle_group = false;
  ControlBlock.auto_cycle = true;
  ControlBlock.fire_mode_group = true;
  ControlBlock.inspect = false;
  ControlBlock.nearest_enemy = false;
  ControlBlock.target_low = false;
  ControlBlock.target_high = true;
  ControlBlock.target_air = false;
  ControlBlock.menu_interface = false;
  ControlBlock.lights = false;
  ControlBlock.enemy = false;
  ControlBlock.enemy_back = false;
  ControlBlock.friendly = false;
  ControlBlock.friendly_back = false;
  ControlBlock.target_pointer = false;
  ControlBlock.nav_point = false;
  ControlBlock.nav_point_back = false;
  ControlBlock.power = true;
  ControlBlock.radar = true;
  ControlBlock.flush_coolant = false;
  ControlBlock.nightvision = false;
  ControlBlock.zoom_in = false;
  ControlBlock.zoom_out = false;
  ControlBlock.zoom_target = false;
  ControlBlock.communication_interface = false;
  ControlBlock.radar_zoom_in = false;
  ControlBlock.radar_zoom_out = false;
  ControlBlock.mfd1_select = false;
  ControlBlock.mfd2_select = false;
  ControlBlock.mfd_up = false;
  ControlBlock.mfd_down = false;
  ControlBlock.mfd_left = false;
  ControlBlock.mfd_right = false;
  ControlBlock.mfd_toggle = false;
  ControlBlock.log = false;
}

/** @brief speeds up and slows down
 */
void GameController::setThrottle(Real a_throttle)
{
  Throttle = a_throttle;

  // cap the throttle to <-1, 1>
  if (Throttle > 1) {
    Throttle = 1;
  } else if (Throttle < -1) {
    Throttle = -1;
  }
}
