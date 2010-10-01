//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef RADARCOMPUTER_H
#define RADARCOMPUTER_H

#include "main.h"
//#include "hud_design.h"
#include "sphere.h"

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

class Corpus;
class Mobilis;
class Unit;
class GameController;

class RadarComputer
{
public:
    RadarComputer(const string& filename, Unit* a_unit);
    ~RadarComputer() { };

    //main loop
    void update(Ogre::Real a_dt);

    //make active
    void activate(bool a_toggle) { active = a_toggle; };

    //toggle radar active of passive
    void setActiveRadar(bool a_toggle) { active_radar = a_toggle; };

    //radar range
    Ogre::Real getRadarRange(Ogre::Real a_range) { return radar_sphere.radius; };

private:
    //main loop
    void updateRadarData();

    //find potential objects to detect
    void updateObjectsWithinRadius();

    //radar on/off
    bool active;

    //spec
    radar_design_t radar_design;
    //owner
    Unit* unit;
    GameController* controller;

    //radar currently in passive/active mode
    bool active_radar;
    //current radar range
    usint range_index;
    Sphere radar_sphere;

    //for checking units only periodically
    Ogre::Real units_refresh_interval;
    Ogre::Real units_refresh_accumulator;

    //objects within the radar's radius
    list<Corpus*> corpus_list;
    list<Mobilis*> mobilis_list;
    list<Unit*> unit_list;
};

#endif // RADARCOMPUTER_H

