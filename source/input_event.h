//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef INPUTEVENT_H
#define INPUTEVENT_H

enum input_event {
    input_event_up, //basic
    input_event_down,
    input_event_left,
    input_event_right,
    input_event_set_speed_reverse, //throttle
    input_event_set_speed_0,
    input_event_set_speed_1,
    input_event_set_speed_2,
    input_event_set_speed_3,
    input_event_set_speed_4,
    input_event_set_speed_5,
    input_event_set_speed_6,
    input_event_set_speed_7,
    input_event_set_speed_8,
    input_event_set_speed_9,
    input_event_set_speed_10,
    input_event_fire, //fire
    input_event_turn_to_pointer, //movement
    input_event_crouch,
    input_event_align_to_torso,
    input_event_nearest_enemy, //target selection
    input_event_enemy,
    input_event_enemy_back,
    input_event_friendly,
    input_event_friendly_back,
    input_event_target_pointer,
    input_event_nav_point,
    input_event_nav_point_back,
    input_event_inspect,
    input_event_fire_mode_group, //firing modes
    input_event_auto_cycle,
    input_event_cycle_weapon,
    input_event_cycle_group,
    input_event_fire_group_1,
    input_event_fire_group_2,
    input_event_fire_group_3,
    input_event_fire_group_4,
    input_event_fire_group_5,
    input_event_fire_group_all,
    input_event_target_low, //fire targetting
    input_event_target_high,
    input_event_target_air,
    input_event_power, //tactical
    input_event_radar,
    input_event_flush_coolant,
    input_event_lights,
    input_event_nightvision,
    input_event_zoom_in, //camera
    input_event_zoom_out,
    input_event_zoom_target,
    input_event_menu_interface, //misc
    input_event_communication_interface,
    input_event_pause,
    input_event_radar_zoom_in,
    input_event_radar_zoom_out,
    input_event_mfd1_select, //mfd
    input_event_mfd2_select,
    input_event_mfd_up,
    input_event_mfd_down,
    input_event_mfd_left,
    input_event_mfd_right,
    input_event_mfd_toggle,
    input_event_log,
};

#endif // INPUTEVENT_H
