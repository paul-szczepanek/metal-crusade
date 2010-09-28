//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef MFD_DISPLAY_H
#define MFD_DISPLAY_H

#include "hud_part.h"

class MFDComputer;
class MFDView;

class HudMFDDisplay : public HudPart
{
public:
    HudMFDDisplay(hud_part_design_t& a_hud_part_design);
    ~HudMFDDisplay();

    //main loop
    void update(Ogre::Real a_dt);

private:
    void changeView(mfd_view::view_type a_view);

    usint font_size;

    //the controlling mfd
    MFDComputer* mfd;

    //for lower fps
    Ogre::Real hud_part_interval;
    Ogre::Real hud_part_accumulator;

    //views
    vector<MFDView*> views;
    mfd_view::view_type current_view;

    //view damage
    vector<Ogre::OverlayElement*> view_dmg_elements;
};

#endif // MFD_DISPLAY_H
