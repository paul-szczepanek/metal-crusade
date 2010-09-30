//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef HUD_SET_H
#define HUD_SET_H

#include "hud_part.h"

class GameController;

class HudSet : public HudPart
{
public:
    HudSet(hud_part_design_t& a_hud_part_design);
    ~HudSet() { };

    //main loop
    void update(Ogre::Real a_dt);

private:
    vector<Ogre::OverlayElement*> items;
    usint item_to_show;

    //change displayed item
    void switchItems(usint a_item_to_show);

    //set functions
    static usint getRadarPower();
    static usint getClockTick();
    static usint getWeaponGroup();
    static usint getWeaponTarget();
    static usint getWeaponAuto();
    static usint getValueZero();
    //function pointer bound to at creation
    usint (*getItemToShow)();
};

#endif // HUD_SET_H
