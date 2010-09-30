//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef HUD_H
#define HUD_H

#include "main.h"
#include "hud_design.h"

typedef Ogre::TextureUnitState::TextureAddressingMode texture_addressing;

class HudPart;
class Unit;
class GameController;
class Timer;
class MFDComputer;

class Hud
{
public:
    Hud();
    ~Hud();
    //init
    void loadHud(Unit* a_player_unit);
    void activate(bool a_toggle);

    //main loop
    void update(Ogre::Real a_dt);

    //called by input manager when the window resizes
    void resize(unsigned int a_screen_width, unsigned int a_screen_height);

    //hookup for hud parts
    void addContainer(hud_area a_hud_area, Ogre::OverlayContainer* a_container) {
        hud_overlays[a_hud_area]->add2D(a_container);
    };
    void addElement(hud_area a_hud_area, Ogre::SceneNode* a_scene_node) {
        hud_overlay_3d->add3D(a_scene_node);
    };

    //communicating with log and status display
    void addLogMessage(const string& a_message);
    void overwriteLogMessage(const string& a_message);
    void setStatusLine(const string& a_message, unsigned short int a_line,
                       unsigned short int a_length = 0, unsigned short int a_position = 0);
    string getStatusLine(unsigned short int a_i, unsigned short int a_j) {
        return status_lines[a_i][a_j];
    };
    string getLogLine(unsigned short int a_i, unsigned short int a_j) {
        return log_lines[a_i][a_j];
    };
    unsigned int getCurrentLogLine() { return current_log_line; };

    //helper functions
    static Ogre::MaterialPtr createOverlayMaterial(const string& a_name,
                                                   texture_addressing a_addressing
                                                   = Ogre::TextureUnitState::TAM_CLAMP);
    Ogre::Real getHudAreaOriginX(hud_area a_hud_area);
    Ogre::Real getHudAreaOriginY(hud_area a_hud_area);

    //hud design and other objects exposed for use in hud parts
    hud_design_t hud_design;
    Unit* player_unit;
    GameController* controller;
    Timer* timer;

private:
    //positioning
    void offsetUpdate(Ogre::Real a_dt, hud_area a_hud_area, bool a_alternative);
    Ogre::Real positionHorizontal(int a_width, horizontal::position a_position, Ogre::Real a_offset);
    Ogre::Real positionVertical(int a_height, vertical::position a_position, Ogre::Real a_offset);

    //hud areas
    Ogre::OverlayContainer* hud_areas[hud_num_of_areas];
    Ogre::Overlay* hud_overlays[hud_num_of_areas];
    Ogre::Overlay* hud_overlay_3d;

    //hud
    bool active;
    Ogre::Real hud_width;
    Ogre::Real hud_height;
    Ogre::Real scale;
    Ogre::Real scale_w;
    Ogre::Real scale_h;

    //MFD
    MFDComputer* mfd1;
    MFDComputer* mfd2;

    //log and status display
    unsigned int current_log_line;
    int_pair area_offsets[hud_num_of_areas];
    char status_lines[hud_num_of_colours][hud_num_of_status_lines][hud_status_line_length + 1];
    char log_lines[hud_num_of_colours][hud_max_hud_num_of_log_lines][hud_log_line_length + 1];

    //all the individual hud parts
    vector<HudPart*> hud_parts;
};

const string hud_textures_dir = "data/textures/hud/";

//hud designs in pixel coords baselined at 1024x768
const Ogre::Real base_width(1024);
const Ogre::Real base_height(768);

#endif // HUD_H

