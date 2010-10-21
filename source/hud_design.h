//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef HUD_DESIGN_H_INCLUDED
#define HUD_DESIGN_H_INCLUDED

#include "mfd_view_design.h"

//each hud must provide 8 parts so that they can surround the screen fully and adopt to scaling
const usint hud_num_of_areas = 8;

//they can be used in a custom way though and the names are just to satisfy Ogre resource names req
enum hud_area {
    hud_mfd1,
    hud_mfd2,
    hud_radar,
    hud_weapon,
    hud_heat,
    hud_speed,
    hud_status,
    hud_log
};

const string hud_area_names[hud_num_of_areas] = {
    "hud_mfd1",
    "hud_mfd2",
    "hud_radar",
    "hud_weapon",
    "hud_heat",
    "hud_speed",
    "hud_status",
    "hud_log"
};

//positioning for hud areas
namespace horizontal { enum position { left, centre, right }; };
namespace vertical { enum position { top, centre, bottom }; };

//this is the minimum number of lines hud must implement to be usable
const usint hud_num_of_log_lines = 8;
const usint hud_num_of_status_lines = 8;
const usint hud_num_of_mfd_aux_lines = 5;

//mfd
const usint view_types_per_page = hud_num_of_mfd_aux_lines - 1; //one line is the label

//text lines max length
const usint hud_log_line_length = 60;
const usint hud_status_line_length = 60;
//history size for the log TODO: write excess to file?
const usint hud_max_hud_num_of_log_lines = 40;
const usint hud_mfd_aux_line_length = 16;

//colour codes for text: regular, exclamation, alternative
//huds can implement them however they want or not at all
const usint hud_num_of_colours = 3; //this is now pretty much hardcoded in parseColours :(
const char hud_colour_codes[hud_num_of_colours] = { 'r', 'e', 'a' };
const char hud_escape_char = '$'; //to get the '$' char in a string type two like so "$$"
const char hud_revert_colour_char = 'x'; //reverts to the last used colour in the line

//an unfortunate name for a namespace but I ran out of good ones
//and I don't want to pollute the namespace even further
//since hud design needs to be used in quite a few places
namespace hud_part_enum {
    enum function {
        mfd,
        mfd_aux,
        radar,
        radar_power,
        clock_tick,
        direction,
        torso_direction,
        compass,
        clock,
        radar_range,
        weapon_group,
        weapon_target,
        weapon_auto,
        weapons,
        log,
        temperature,
        temperature_external,
        coolant,
        speed,
        throttle,
        pressure,
        temperature_engine,
        status
    };

    enum type {
        mfd_military,
        mfd_aux_military,
        radar_military,
        set,
        pointer,
        text,
        weapon_tab,
        log_printer,
        dial,
        status_display,
    };
};

struct hud_part_design_t {
    hud_area area;
    string name;
    hud_part_enum::type type;
    hud_part_enum::function function;
    int_pair position;
    int_pair size;
    vector<Ogre::Real> parameters;
};

struct hud_design_t {
    string name;
    string font_name;
    vector<string> area_textures;
    vector<string> area_cover_textures;
    vector<pair<horizontal::position, vertical::position> > positions;
    vector<int_pair> sizes;
    vector<int_pair> offsets;
    vector<int_pair> offsets_alternative;
    vector<Ogre::ColourValue> status_colours;
    vector<Ogre::ColourValue> log_colours;
    vector<Ogre::ColourValue> mfd_colours;
    vector<Ogre::ColourValue> display_colours;
    vector<hud_part_design_t> parts;
    vector<mfd_view::view_type> mfd_views;
};

#endif // HUD_DESIGN_H_INCLUDED
