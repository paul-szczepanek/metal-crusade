// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "main.h"

// TODO: bitset this later (or possibly only do that for sending over the network)
struct control_block_t {
  bool fire;
  bool turn_to_pointer;
  bool crouch;
  bool align_to_torso;
  bool fire_group_1;
  bool fire_group_2;
  bool fire_group_3;
  bool fire_group_4;
  bool fire_group_5;
  bool fire_group_all;
  bool cycle_weapon;
  bool cycle_group;
  bool auto_cycle;
  bool fire_mode_group;
  bool inspect;
  bool nearest_enemy;
  bool target_low;
  bool target_high;
  bool target_air;
  bool menu_interface;
  bool lights;
  bool enemy;
  bool enemy_back;
  bool friendly;
  bool friendly_back;
  bool target_pointer;
  bool nav_point;
  bool nav_point_back;
  bool power;
  bool radar;
  bool flush_coolant;
  bool nightvision;
  bool zoom_in;
  bool zoom_out;
  bool zoom_target;
  bool communication_interface;
  bool radar_zoom_in;
  bool radar_zoom_out;
  bool mfd1_select;
  bool mfd2_select;
  bool mfd_up;
  bool mfd_down;
  bool mfd_left;
  bool mfd_right;
  bool mfd_toggle;
  bool log;
};

class Formation;

class GameController
{
public:
  GameController(const string& a_name);
  ~GameController() {
  }

  // throttle
  void setThrottle(Real a_throttle);
  Real getThrottle() {
    return throttle;
  }

  // turning the body
  void setTurn(Real a_turn_speed) {
    turn_speed = a_turn_speed;
  }

  Real getTurnSpeed() {
    return turn_speed;
  }

  // 3d pointer
  void setPointerPos(Vector3 a_pointer_pos) {
    pointer_pos = a_pointer_pos;
  }

  Vector3 getPointerPos() {
    return pointer_pos;
  }

  Vector2 getPointerPosXZ() {
    return Vector2(pointer_pos.x, pointer_pos.z);
  }

  string getName() {
    return name;
  }

  // formation (and thus faction) affiliation
  Formation* getFormation() {
    return formation;
  }

  void setFormation(Formation* a_formation) {
    formation = a_formation;
  }

  void setFormation(string a_formation_name);

  control_block_t control_block;

private:
  // speed
  Real turn_speed;
  Real throttle;

  // pinter
  Vector3 pointer_pos;

  // controller name being the actual player name
  string name;

  // group affiliation
  Formation* formation;

  // control options
  bool InvertTurnToPointer;
};

const Real target_air_offset = 64;
const Real target_high_offset = 10;
const Real target_low_offset = 4;

#endif // GAMECONTROLLER_H
