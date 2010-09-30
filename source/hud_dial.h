//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef HUD_DIAL_H
#define HUD_DIAL_H

#include "hud_part.h"

// parameters
// (min_value,
// max_value,
// min_angle,
// max_angle,
// direction) //+1 is clockwise, -1 is counterclockwise

class HudDial : public HudPart
{
public:
    HudDial(hud_part_design_t& a_hud_part_design);
    ~HudDial() { };

    //main loop
    void update(Ogre::Real a_dt);

private:
    //dial specs
    Ogre::Real min_value;
    Ogre::Real max_value;
    Ogre::Real min_angle;
    Ogre::Real max_angle;
    Ogre::Real value;
    Ogre::Real angle;
    Ogre::TextureUnitState* dial_texture;

    //dial functions
    static Ogre::Real getValueSpeed();
    static Ogre::Real getValuePressure();
    static Ogre::Real getValueCoolant();
    static Ogre::Real getValueEngineTemperature();
    static Ogre::Real getValueCoreTemperature();
    static Ogre::Real getValueExternalTemperature();
    static Ogre::Real getValueThrottle();
    static Ogre::Real getValueZero();
    //fucntion pointer bound to at creation
    Ogre::Real (*getValue)();
};

#endif // HUD_DIAL_H
