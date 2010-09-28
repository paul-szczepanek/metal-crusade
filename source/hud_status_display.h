//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef HUD_STATUS_DISPLAY_H
#define HUD_STATUS_DISPLAY_H

#include "hud_part.h"

class HudStatusDisplay : public HudPart
{
public:
    HudStatusDisplay(hud_part_design_t& a_hud_part_design);
    ~HudStatusDisplay() { };

    //main loop
    void update(Ogre::Real a_dt);

private:
    usint font_size;

    //for lower fps
    Ogre::Real hud_part_interval;
    Ogre::Real hud_part_accumulator;

    //lines of text
    Ogre::OverlayElement* status_text_elements[hud_num_of_colours][hud_num_of_status_lines];
};

#endif // HUD_STATUS_DISPLAY_H
