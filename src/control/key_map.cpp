// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "key_map.h"
#include "files_handler.h"

/** @brief sets the mappings
  */
KeyMap::KeyMap()
{
  assert(getKeyConfig("controls"));
}

/** @brief loads the config from a file
  */
bool KeyMap::getKeyConfig(const string& filename)
{
  // insert data from file into pairs
  map<string, string> pairs;
  assert(FS::getPairs(filename, CONFIG_DIR, pairs));
  map<input_event, string> enum_pairs;

  // create a map to translate enum to string which indexes the pairs from the file
  enum_pairs.insert(make_pair(input_event_up, "input_event_up"));
  enum_pairs.insert(make_pair(input_event_down, "input_event_down"));
  enum_pairs.insert(make_pair(input_event_left, "input_event_left"));
  enum_pairs.insert(make_pair(input_event_right, "input_event_right"));
  enum_pairs.insert(make_pair(input_event_set_speed_reverse, "input_event_set_speed_reverse"));
  enum_pairs.insert(make_pair(input_event_set_speed_0, "input_event_set_speed_0"));
  enum_pairs.insert(make_pair(input_event_set_speed_1, "input_event_set_speed_1"));
  enum_pairs.insert(make_pair(input_event_set_speed_2, "input_event_set_speed_2"));
  enum_pairs.insert(make_pair(input_event_set_speed_3, "input_event_set_speed_3"));
  enum_pairs.insert(make_pair(input_event_set_speed_4, "input_event_set_speed_4"));
  enum_pairs.insert(make_pair(input_event_set_speed_5, "input_event_set_speed_5"));
  enum_pairs.insert(make_pair(input_event_set_speed_6, "input_event_set_speed_6"));
  enum_pairs.insert(make_pair(input_event_set_speed_7, "input_event_set_speed_7"));
  enum_pairs.insert(make_pair(input_event_set_speed_8, "input_event_set_speed_8"));
  enum_pairs.insert(make_pair(input_event_set_speed_9, "input_event_set_speed_9"));
  enum_pairs.insert(make_pair(input_event_set_speed_10, "input_event_set_speed_10"));
  enum_pairs.insert(make_pair(input_event_fire, "input_event_fire"));
  enum_pairs.insert(make_pair(input_event_turn_to_pointer, "input_event_turn_to_pointer"));
  enum_pairs.insert(make_pair(input_event_crouch, "input_event_crouch"));
  enum_pairs.insert(make_pair(input_event_align_to_torso, "input_event_align_to_torso"));
  enum_pairs.insert(make_pair(input_event_nearest_enemy, "input_event_nearest_enemy"));
  enum_pairs.insert(make_pair(input_event_enemy, "input_event_enemy"));
  enum_pairs.insert(make_pair(input_event_enemy_back, "input_event_enemy_back"));
  enum_pairs.insert(make_pair(input_event_friendly, "input_event_friendly"));
  enum_pairs.insert(make_pair(input_event_friendly_back, "input_event_friendly_back"));
  enum_pairs.insert(make_pair(input_event_target_pointer, "input_event_target_pointer"));
  enum_pairs.insert(make_pair(input_event_nav_point, "input_event_nav_point"));
  enum_pairs.insert(make_pair(input_event_nav_point_back, "input_event_nav_point_back"));
  enum_pairs.insert(make_pair(input_event_inspect, "input_event_inspect"));
  enum_pairs.insert(make_pair(input_event_fire_mode_group, "input_event_fire_mode_group"));
  enum_pairs.insert(make_pair(input_event_auto_cycle, "input_event_auto_cycle"));
  enum_pairs.insert(make_pair(input_event_cycle_weapon, "input_event_cycle_weapon"));
  enum_pairs.insert(make_pair(input_event_cycle_group, "input_event_cycle_group"));
  enum_pairs.insert(make_pair(input_event_fire_group_1, "input_event_fire_group_1"));
  enum_pairs.insert(make_pair(input_event_fire_group_2, "input_event_fire_group_2"));
  enum_pairs.insert(make_pair(input_event_fire_group_3, "input_event_fire_group_3"));
  enum_pairs.insert(make_pair(input_event_fire_group_4, "input_event_fire_group_4"));
  enum_pairs.insert(make_pair(input_event_fire_group_5, "input_event_fire_group_5"));
  enum_pairs.insert(make_pair(input_event_fire_group_all, "input_event_fire_group_all"));
  enum_pairs.insert(make_pair(input_event_target_low, "input_event_target_low"));
  enum_pairs.insert(make_pair(input_event_target_high, "input_event_target_high"));
  enum_pairs.insert(make_pair(input_event_target_air, "input_event_target_air"));
  enum_pairs.insert(make_pair(input_event_power, "input_event_power"));
  enum_pairs.insert(make_pair(input_event_radar, "input_event_radar"));
  enum_pairs.insert(make_pair(input_event_flush_coolant, "input_event_flush_coolant"));
  enum_pairs.insert(make_pair(input_event_lights, "input_event_lights"));
  enum_pairs.insert(make_pair(input_event_nightvision, "input_event_nightvision"));
  enum_pairs.insert(make_pair(input_event_zoom_in, "input_event_zoom_in"));
  enum_pairs.insert(make_pair(input_event_zoom_out, "input_event_zoom_out"));
  enum_pairs.insert(make_pair(input_event_zoom_target, "input_event_zoom_target"));
  enum_pairs.insert(make_pair(input_event_menu_interface, "input_event_menu_interface"));
  enum_pairs.insert(make_pair(input_event_communication_interface,
                              "input_event_communication_interface"));
  enum_pairs.insert(make_pair(input_event_pause, "input_event_pause"));
  enum_pairs.insert(make_pair(input_event_radar_zoom_in, "input_event_radar_zoom_in"));
  enum_pairs.insert(make_pair(input_event_radar_zoom_out, "input_event_radar_zoom_out"));
  enum_pairs.insert(make_pair(input_event_mfd1_select, "input_event_mfd1_select"));
  enum_pairs.insert(make_pair(input_event_mfd2_select, "input_event_mfd2_select"));
  enum_pairs.insert(make_pair(input_event_mfd_up, "input_event_mfd_up"));
  enum_pairs.insert(make_pair(input_event_mfd_down, "input_event_mfd_down"));
  enum_pairs.insert(make_pair(input_event_mfd_left, "input_event_mfd_left"));
  enum_pairs.insert(make_pair(input_event_mfd_right, "input_event_mfd_right"));
  enum_pairs.insert(make_pair(input_event_mfd_toggle, "input_event_mfd_toggle"));
  enum_pairs.insert(make_pair(input_event_log, "input_event_log"));

  // iterate through all input enums and insert keycodes or mouseids to Keys and Buttons
  for(uint i = 0, for_size = enum_pairs.size(); i < for_size; ++i) {
    // get OIS keycode
    OIS::KeyCode key = translateKeycode(pairs[enum_pairs[input_event(i)]]);

    // if it's a valid key
    if(key != OIS::KC_UNASSIGNED) {
      // assign it to the input event
      Keys.insert(make_pair(input_event(i), key));
      // disable mouse button
      Buttons.insert(make_pair(input_event(i), OIS::MB_Button7));

    } else {
      // otherwise get a mouse id
      OIS::MouseButtonID mouseid = translateMouse(pairs[enum_pairs[input_event(i)]]);

      // if it's a valid mouse id
      if(mouseid != OIS::MB_Button7) {
        // assign it to the input event
        Buttons.insert(make_pair(input_event(i), mouseid));
        // disable key
        Keys.insert(make_pair(input_event(i), OIS::KC_UNASSIGNED));

      } else {
        // TODO: fallback to default config;
        assert(0 == "the key map is garbled");
        return false;
      }
    }
  }

  return true;
}

/** @brief translates from human readable to OIS ids
  */
OIS::KeyCode KeyMap::translateKeycode(const string& id)
{
  map<string, OIS::KeyCode> keys;

  // build map to translate string to OIS keycode
  keys["ESCAPE"] = OIS::KC_ESCAPE;
  keys["1"] = OIS::KC_1;
  keys["2"] = OIS::KC_2;
  keys["3"] = OIS::KC_3;
  keys["4"] = OIS::KC_4;
  keys["5"] = OIS::KC_5;
  keys["6"] = OIS::KC_6;
  keys["7"] = OIS::KC_7;
  keys["8"] = OIS::KC_8;
  keys["9"] = OIS::KC_9;
  keys["0"] = OIS::KC_0;
  keys["Q"] = OIS::KC_Q;
  keys["W"] = OIS::KC_W;
  keys["E"] = OIS::KC_E;
  keys["R"] = OIS::KC_R;
  keys["T"] = OIS::KC_T;
  keys["Y"] = OIS::KC_Y;
  keys["U"] = OIS::KC_U;
  keys["I"] = OIS::KC_I;
  keys["O"] = OIS::KC_O;
  keys["P"] = OIS::KC_P;
  keys["A"] = OIS::KC_A;
  keys["S"] = OIS::KC_S;
  keys["D"] = OIS::KC_D;
  keys["F"] = OIS::KC_F;
  keys["G"] = OIS::KC_G;
  keys["H"] = OIS::KC_H;
  keys["J"] = OIS::KC_J;
  keys["K"] = OIS::KC_K;
  keys["L"] = OIS::KC_L;
  keys["Z"] = OIS::KC_Z;
  keys["X"] = OIS::KC_X;
  keys["C"] = OIS::KC_C;
  keys["V"] = OIS::KC_V;
  keys["B"] = OIS::KC_B;
  keys["N"] = OIS::KC_N;
  keys["M"] = OIS::KC_M;
  keys["F1"] = OIS::KC_F1;
  keys["F2"] = OIS::KC_F2;
  keys["F3"] = OIS::KC_F3;
  keys["F4"] = OIS::KC_F4;
  keys["F5"] = OIS::KC_F5;
  keys["F6"] = OIS::KC_F6;
  keys["F7"] = OIS::KC_F7;
  keys["F8"] = OIS::KC_F8;
  keys["F9"] = OIS::KC_F9;
  keys["F10"] = OIS::KC_F10;
  keys["LBRACKET"] = OIS::KC_LBRACKET;
  keys["RBRACKET"] = OIS::KC_RBRACKET;
  keys["RETURN"] = OIS::KC_RETURN;
  keys["LCONTROL"] = OIS::KC_LCONTROL;
  keys["MINUS"] = OIS::KC_MINUS;
  keys["EQUALS"] = OIS::KC_EQUALS;
  keys["BACK"] = OIS::KC_BACK;
  keys["TAB"] = OIS::KC_TAB;
  keys["SEMICOLON"] = OIS::KC_SEMICOLON;
  keys["APOSTROPHE"] = OIS::KC_APOSTROPHE;
  keys["GRAVE"] = OIS::KC_GRAVE;
  keys["LSHIFT"] = OIS::KC_LSHIFT;
  keys["BACKSLASH"] = OIS::KC_BACKSLASH;
  keys["COMMA"] = OIS::KC_COMMA;
  keys["PERIOD"] = OIS::KC_PERIOD;
  keys["SLASH"] = OIS::KC_SLASH;
  keys["RSHIFT"] = OIS::KC_RSHIFT;
  keys["MULTIPLY"] = OIS::KC_MULTIPLY;
  keys["LMENU"] = OIS::KC_LMENU;
  keys["SPACE"] = OIS::KC_SPACE;
  keys["CAPITAL"] = OIS::KC_CAPITAL;
  keys["NUMLOCK"] = OIS::KC_NUMLOCK; // numpad
  keys["NUMPADCOMMA"] = OIS::KC_NUMPADCOMMA;
  keys["SCROLL"] = OIS::KC_SCROLL;
  keys["NUMPAD7"] = OIS::KC_NUMPAD7;
  keys["NUMPAD8"] = OIS::KC_NUMPAD8;
  keys["NUMPAD9"] = OIS::KC_NUMPAD9;
  keys["SUBTRACT"] = OIS::KC_SUBTRACT;
  keys["NUMPAD4"] = OIS::KC_NUMPAD4;
  keys["NUMPAD5"] = OIS::KC_NUMPAD5;
  keys["NUMPAD6"] = OIS::KC_NUMPAD6;
  keys["ADD"] = OIS::KC_ADD;
  keys["NUMPAD1"] = OIS::KC_NUMPAD1;
  keys["NUMPAD2"] = OIS::KC_NUMPAD2;
  keys["NUMPAD3"] = OIS::KC_NUMPAD3;
  keys["NUMPAD0"] = OIS::KC_NUMPAD0;
  keys["DECIMAL"] = OIS::KC_DECIMAL;
  keys["RCONTROL"] = OIS::KC_RCONTROL;
  keys["DIVIDE"] = OIS::KC_DIVIDE;
  keys["SYSRQ"] = OIS::KC_SYSRQ;
  keys["RMENU"] = OIS::KC_RMENU;
  keys["PAUSE"] = OIS::KC_PAUSE;
  keys["HOME"] = OIS::KC_HOME;
  keys["UP"] = OIS::KC_UP;
  keys["PGUP"] = OIS::KC_PGUP;
  keys["LEFT"] = OIS::KC_LEFT;
  keys["RIGHT"] = OIS::KC_RIGHT;
  keys["END"] = OIS::KC_END;
  keys["DOWN"] = OIS::KC_DOWN;
  keys["PGDOWN"] = OIS::KC_PGDOWN;
  keys["INSERT"] = OIS::KC_INSERT;
  keys["DELETE"] = OIS::KC_DELETE;
  keys["LWIN"] = OIS::KC_LWIN;
  keys["RWIN"] = OIS::KC_RWIN;
  keys["APPS"] = OIS::KC_APPS;

  // return unassaigned if key is not on the list
  if(keys.find(id) == keys.end()) {
    return OIS::KC_UNASSIGNED;

  } else {
    return keys[id];
  }
}

/** @brief translates from human readable to OIS ids
  */
OIS::MouseButtonID KeyMap::translateMouse(const string& id)
{
  map<string, OIS::MouseButtonID> mouse;

  // build map to translate string to OIS mouse id
  mouse["MOUSE_LEFT"] = OIS::MB_Left;
  mouse["MOUSE_MIDDLE"] = OIS::MB_Middle;
  mouse["MOUSE_RIGHT"] = OIS::MB_Right;
  mouse["MOUSE_BUTTON3"] = OIS::MB_Button3;
  mouse["MOUSE_BUTTON4"] = OIS::MB_Button4;
  mouse["MOUSE_BUTTON5"] = OIS::MB_Button5;
  mouse["MOUSE_BUTTON6"] = OIS::MB_Button6;

  // return button7 which is neer used if id is not on the list
  if(mouse.find(id) == mouse.end()) {
    return OIS::MB_Button7;

  } else {
    return mouse[id];
  }
}
