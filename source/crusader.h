//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef CRUSADER_H
#define CRUSADER_H

#include "unit.h"
#include "internal_type.h"
#include "crusader_design.h"
#include "mfd_view_design.h"

class Weapon;
class Animation;

class Crusader : public Unit
{
public:
    Crusader(Ogre::Vector3 a_pos_xyz, const string& a_unit_name,
             Ogre::SceneNode* a_scene_node, Ogre::Quaternion a_orientation,
             crusader_design_t a_design, crusader_engine_t a_engine, crusader_drive_t a_drive,
             crusader_chasis_t a_chasis, crusader_model_t a_model);
    ~Crusader();

    //main loop
    int updateController();

    //collision
    int handleCollision(Collision* a_collision);

    //directions
    Ogre::Vector3 getDirection() { return torso_direction; };
    Ogre::Vector3 getDriveDirection() { return direction; };
    Ogre::Quaternion getLookingOrientation() { return orientation * torso_orientation; };

    //ramming damage
    Ogre::Real getBallisticDmg();

    //hud
    string getHudName() { return design.hud; };
    Ogre::Real getSpeed() { return corrected_velocity_scalar; };
    Ogre::Real getCoolant() { return coolant_level; };
    Ogre::Real getEngineTemperature() { return engine_temperature; };
    Ogre::Real getCoreTemperature() { return core_temperature; };
    Ogre::Real getSurfaceTemperature() { return surface_temperature; };
    Ogre::Real getPressure() { return core_integrity; };
    Ogre::Real getThrottle();

    //damage reporting
    Ogre::Real getDamage(usint a_diagram_element);
    mfd_view::diagram_type getDiagramType() { return mfd_view::biped_crusader; };

    //weapons
    inline vector<usint>& getSelectedGroup();
    inline usint getSelectedWeapon();

private:
    //inner main loop
    void moveCrusader();
    void moveTorso();
    void fireWeapons();
    void pumpHeat();
    void shockDamage();
    void updateAnimations();

    //helper functions
    void localiseAngle(Ogre::Radian& angle, const Ogre::Radian& global_angle);

    //building the crusader
    void recalculateWeight();
    void extractAnimationStates(Ogre::SceneNode* a_scene_node);
    void positionWeapons(vector<Ogre::Vector3>& panel_positions, vector<usint>& slots_used);

    //weapons
    bool fireGroup(usint a_group);
    void cycleGroup();
    void cycleWeapon();
    bool weapons_operational;

    usint current_group;
    usint current_weapon;

    //tracking G forces
    Ogre::Vector3 shock_damage_old;
    Ogre::Vector3 shock_damage_new;

    //design
    crusader_design_t design;
    crusader_engine_t engine;
    crusader_drive_t drive;
    crusader_chasis_t chasis;
    crusader_model_t model;

    //effective heatsinks
    usint heatsinks;

    //node that rotates the torso
    Ogre::SceneNode* torso_node;

    //moving
    Ogre::Real throttle;

    //orientation
    int angular_momentum_top;
    Ogre::Quaternion torso_orientation;
    Ogre::Vector3 torso_direction;

    //distance to ground from main scene node
    Ogre::Real crusader_height;

    //clamping to the terrain
    Ogre::Ray* terrain_ray;
    Ogre::RaySceneQuery* terrain_ray_query;

    //integrity and armour
    vector<Ogre::Real> structure;
    vector<Ogre::Real> armour;
    Ogre::Real armour_structure;
    Ogre::Real armour_ballistic;
    Ogre::Real armour_conductivity;
    Ogre::Real armour_generated_heat;

    //temperature
    Ogre::Real coolant_level;
    Ogre::Real coolant;
    Ogre::Real engine_temperature;

    //animation
    Animation* animation;
};

const Ogre::Real critical_temperature(500);

inline usint Crusader::getSelectedWeapon()
{
    return weapons_operational? design.weapon_groups[current_group][current_weapon] : -1;
}

inline vector<usint>& Crusader::getSelectedGroup()
{
    return design.weapon_groups[current_group];
}

inline Ogre::Real Crusader::getBallisticDmg()
{
    Ogre::Real dmg = log10(velocity.length());
    if (dmg > 0) {
        return dmg * penetration;
    } else {
        return 0;
    }
}

inline Ogre::Real Crusader::getThrottle()
{
    //throttle scaling depends on direction
    return (throttle > 0)? throttle * drive.max_speed : throttle * drive.max_speed_reverse;
}

inline Ogre::Real Crusader::getDamage(usint a_diagram_element)
{
    return armour[a_diagram_element] / design.armour_placement[a_diagram_element];
}

#endif // CRUSADER_H
