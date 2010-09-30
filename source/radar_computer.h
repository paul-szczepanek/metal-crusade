//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef RADARCOMPUTER_H
#define RADARCOMPUTER_H

#include "main.h"
#include "hud_design.h"

namespace radar {
    enum sweep_type {
        rotating,
        fixed,
        oscilating
    };
};

struct radar_design_t {
    string filename;
    string model;
    bool active;
    Ogre::Real power;
    Ogre::Real heat_sensivity;
    Ogre::Real electromagnetic_sensivity;
    Ogre::Real weight;
    radar::sweep_type sweep;
    Ogre::Radian cone_angle;
    usint heads;
    //strings
    ulint text_name;
    ulint text_description;
    ulint text_list_name;
};

class RadarComputer
{
public:
    RadarComputer();
    ~RadarComputer() { };

    //main loop
    void update(Ogre::Real a_dt);

    //make active
    void activate(bool a_toggle) { active = a_toggle; };

    //toggle radar active of passive
    void setActiveRadar(bool a_toggle) { active_radar = a_toggle; };

    //radar range
    void setRadarRange(Ogre::Real a_range) { range = a_range; };

private:
    //radar on/off
    bool active;

    //radar passive/active
    bool active_radar;
    //radar range
    Ogre::Real range;
};

#endif // RADARCOMPUTER_H

