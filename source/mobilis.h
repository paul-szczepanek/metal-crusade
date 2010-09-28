//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef MOBILIS_H
#define MOBILIS_H

#include "corpus.h"
#include "mfd_view_design.h"

class Mobilis : public Corpus
{
public:
    Mobilis(Ogre::Vector3 a_pos_xyz, const string& a_unit_name,
            Ogre::SceneNode* a_scene_node, Ogre::Quaternion a_orientation);
    virtual ~Mobilis();

    //main loop
    int update(Ogre::Real a_dt);

    //move around
    void setXYZ(Ogre::Vector3 a_pos_xyz) { pos_xyz = a_pos_xyz; };
    void setVelocity(Ogre::Vector3 a_velocity) { velocity = a_velocity; };
    Ogre::Vector3 getVelocity() { return velocity; };

    //resolve collisions
    virtual int handleCollision(Collision* a_collision);
    bool revertMove(Ogre::Vector3 a_move);

    //weight
    Ogre::Real getWeight() { return total_weight; };

    //targetting

    //called by other object to try and aqcuire this as a target
    bool acquireAsTarget(Mobilis* a_targeted_by);
    //called by other objects which hold this as a target to let it know that they no longer do
    void releaseAsTarget(Mobilis* a_targeted_by);
    //called by targeted object that requires this to relinquish its current target
    bool loseTarget(Mobilis* a_targeted_by, bool a_forced = false);

    //return the target of this
    Mobilis* getTarget() { return target; };

    //damage reporting
    virtual Ogre::Real getDamage(usint a_diagram_element) { return core_integrity; };
    virtual mfd_view::diagram_type getDiagramType() { return mfd_view::object; };

    //hud operation
    virtual Ogre::Real getSpeed() { return corrected_velocity_scalar; };
    virtual Ogre::Real getEngineTemperature()  { return 0; };
    virtual Ogre::Real getCoreTemperature()  { return 0; };
    virtual Ogre::Real getPressure()  { return core_integrity; };
    Ogre::Real getSurfaceTemperature() { return surface_temperature; };

    //partitionening into cells on the arena made
    virtual void updateCellIndex();

protected:
    //main loop
    virtual int updateController();

    //clear targets when this object gets destroyed
    void clearFromTargets();

    //speed and position
    Ogre::Vector3 velocity;
    Ogre::Vector3 move;
    Ogre::Real angular_momentum;
    Ogre::Real corrected_velocity_scalar;
    bool out_of_bounds;

    //weight
    Ogre::Real total_weight;

    //collision system
    bool registered;

    //hit points
    Ogre::Real core_integrity;

    //targeted objects
    Mobilis* target;

    //targeted by objects
    vector<Mobilis*> target_holders;
};

#endif // MOBILIS_H
