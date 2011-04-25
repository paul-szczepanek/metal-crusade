//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef HUD_H
#define HUD_H

#include "main.h"
#include "hud_design.h"

class HudPart;
class Unit;
class GameController;
class Timer;
class MFDComputer;
class LogComputer;
class StatusComputer;
class RadarComputer;

namespace interface_mode {
    enum modes {
        mfd,
        log,
        computer,
        communication
    };
};

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

    //shows the pause screen
    void pause();

    //called by input manager when the window resizes
    void resize(unsigned int a_screen_width, unsigned int a_screen_height);

    //hookup for hud parts
    void addContainer(hud_area a_hud_area, Ogre::OverlayContainer* a_container);
    void addElement(Ogre::SceneNode* a_scene_node);

    //parsing colour codes in a string into monocolour lines
    void parseColours(const string& message, usint a_length,
                      char* line1, char* line2, char* line3);

    //hooking up MFDs to their displays
    MFDComputer* getMFD() { return mfds.back(); };
    uint getMFDNumber() { return mfds.size(); };

    //helper functions
    static Ogre::MaterialPtr createOverlayMaterial(const string& a_name,
                                                   texture_addressing a_addressing
                                                   = Ogre::TextureUnitState::TAM_CLAMP,
                                                   string a_texture_name = string(""));
    Ogre::Real getHudAreaOriginX(hud_area a_hud_area);
    Ogre::Real getHudAreaOriginY(hud_area a_hud_area);

    //log and status display
    LogComputer* log;
    StatusComputer* status;

    //the unit using the hud
    Unit* player_unit;
    GameController* controller;
    RadarComputer* radar;

    //local time
    Timer* timer;

    //hud design
    hud_design_t hud_design;

private:
    //positioning
    void offsetUpdate(Ogre::Real a_dt, hud_area a_hud_area, bool a_alternative);
    Ogre::Real positionHorizontal(int a_width, horizontal::position a_position,
                                  Ogre::Real a_offset);
    Ogre::Real positionVertical(int a_height, vertical::position a_position,
                                Ogre::Real a_offset);

    //hud areas
    Ogre::OverlayContainer* hud_areas[hud_num_of_areas];
    Ogre::Overlay* hud_overlays[hud_num_of_areas];
    Ogre::Overlay* hud_overlay_3d;

    //special temp containers for pause
    Ogre::OverlayContainer* centre_container;
    Ogre::Overlay* centre_overlay;

    //hud size and position
    bool active;
    Ogre::Real hud_width;
    Ogre::Real hud_height;
    Ogre::Real scale;
    Ogre::Real scale_w;
    Ogre::Real scale_h;
    int_pair area_offsets[hud_num_of_areas];

    //all the individual hud parts
    vector<HudPart*> hud_parts;

    //MFD
    vector<MFDComputer*> mfds;
    usint selected_mfd;

    //wchich item is active
    interface_mode::modes hud_mode;
};

inline void Hud::addContainer(hud_area a_hud_area, Ogre::OverlayContainer* a_container)
{
    hud_overlays[a_hud_area]->add2D(a_container);
}

inline void Hud::addElement(Ogre::SceneNode* a_scene_node)
{
    hud_overlay_3d->add3D(a_scene_node);
}

#endif // HUD_H
