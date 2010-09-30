//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef CORPUS_H
#define CORPUS_H

#include "main.h"
#include "collision_type.h"
#include "armour_type.h"
#include "sphere.h"

const usint max_num_cs = 32;
const usint max_num_es = 8;

class Collision;
class GameController;
class Sphere;

struct exclusion_sphere_t {
    Sphere sphere;
    bitset<max_num_cs> cs_bitset;
};

class Corpus
{
public:
    Corpus(Ogre::Vector3 a_pos_xyz, const string& a_unit_name,
           Ogre::SceneNode* a_scene_node, Ogre::Quaternion a_orientation);
    virtual ~Corpus() { };

    //main loop
    virtual int update(Ogre::Real a_dt);

    //position
    Ogre::Real getX() { return pos_xyz.x; };
    Ogre::Real getY() { return pos_xyz.y; };
    Ogre::Real getZ() { return pos_xyz.z; };
    Ogre::Vector2 getXZ() { return Ogre::Vector2(pos_xyz.x, pos_xyz.z); };
    Ogre::Vector3 getPosition() { return pos_xyz; };

    //name (model or unit name, not the actual entity)
    string getName() { return unit_name; };

    //orientation
    Ogre::Quaternion getOrientation() { return orientation; };
    virtual Ogre::Quaternion getLookingOrientation() { return orientation; };
    virtual Ogre::Vector3 getDirection();

    //controllers
    GameController* getController() { return controller; };
    void assignController(GameController* a_controller);
    void removeController();
    void activateController(bool a_toggle);

    //damage and heat
    virtual Ogre::Real getSurfaceTemperature() { return surface_temperature; };
    void setSurfaceTemperature(Ogre::Real a_surface_temperature) ;
    virtual Ogre::Real getBallisticDmg() { return 0; };
    virtual Ogre::Real getEnergyDmg() { return 0; };
    virtual Ogre::Real getHeatDmg() { return 0; };

    //corpus ignores any request to react to collisions
    virtual int handleCollision(Collision* a_collision);
    virtual Ogre::Vector3 getVelocity() { return Ogre::Vector3::ZERO; };
    virtual Ogre::Real getWeight() { return 10000; };
    virtual bool revertMove(Ogre::Vector3 a_move) { return true; };

    //collisions
    collision_type getCollisionType() { return collision; };
    Ogre::Real getPenetration() { return penetration; };
    Ogre::Real getFriction() { return friction; };
    virtual bool validateCollision(Corpus* a_colliding_object) { return true; } ;
    virtual Sphere getBoundingSphere();
    virtual bitset<max_num_es> getExclusionSpheres(Sphere& sphere);
    virtual bitset<max_num_cs> getCollisionSpheres(Sphere& sphere);
    virtual bitset<max_num_cs> getCollisionSpheres(Sphere& sphere, bitset<max_num_es> es_bitset);
    vector<Sphere> collision_spheres;

    //main ogre scene node for the object with synced position
    Ogre::SceneNode* scene_node;

    //put it in the cell on the arena
    void updateCellIndex();

protected:
    //main loop
    virtual int updateController() { return 0; };

    //read collision spheres for the object TEMP
    void loadCollisionSpheres();

    //collision spheres
    vector<exclusion_sphere_t> exclusion_spheres;
    bool bounding_sphere_invalid;
    bitset<max_num_es> es_invalid; //exclusion spheres that need their position recalculated
    bitset<max_num_cs> cs_invalid; //same for collision spheres
    Sphere bounding_sphere;
    vector<usint> cs_areas;
    vector<usint> es_areas;

    //sphere positions relative to main scene node
    Ogre::Vector3 relative_bs_position;
    vector<Ogre::Vector3> relative_es_positions;
    vector<Ogre::Vector3> relative_cs_positions;

    //uniqe id for producing meshes inside corpus
    string getUniqueID();
    static ulint unique_id;

    //time elapsed since last frame
    Ogre::Real dt;

    //position and orientation
    Ogre::Vector3 pos_xyz;
    Ogre::Quaternion orientation;
    Ogre::Vector3 direction;

    //unit name (model)
    string unit_name;

    //controller
    GameController* controller;
    bool controller_active;

    //collistion
    collision_type collision;
    Ogre::Real penetration;
    Ogre::Real surface_temperature;
    Ogre::Real friction;
    Ogre::Real conductivity;

    //index of the cell in the arena the ojbect is in
    uint_pair cell_index;

    //debug
    vector<Ogre::Entity*> debug_cs_entities;
    vector<Ogre::SceneNode*> debug_cs_nodes;
    Ogre::Entity* debug_bs_entity;
    Ogre::SceneNode* debug_bs_node;

private:
    void displayCollision(bool a_toggle);
    void displayCollisionHit();
    void displayCollisionUpdate();
    bool display_collision_debug;
};

inline void Corpus::setSurfaceTemperature(Ogre::Real a_surface_temperature)
{
    surface_temperature = a_surface_temperature;
};

#endif // CORPUS_H
