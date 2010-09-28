//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef ARENA_H
#define ARENA_H

#include "main.h"

//the arena is partitioned into cells to help find things and potential collisions
//TODO: find the best value for the number of cells
const unsigned int num_of_arena_cells = 256;

class GameController;
class Corpus;
class Mobilis;
class Unit;

class Arena
{
public:
    Arena();
    ~Arena();

    //init
    int loadArena(const string& arena_name);
    void loadTerrain();
    void createTerrain();

    //main loop
    void update(Ogre::Real a_dt);

    //get height at x and y
    Ogre::Real getHeight(Ogre::Real a_x, Ogre::Real a_y);

    //temperature at any point on the map
    Ogre::Real getAmbientTemperature(Ogre::Vector3 a_position);

    //gravity
    Ogre::Real getGravity() { return gravity; };

    //the arena is partitioned into cells - reutrns the cell index
    uint_pair getCellIndex(Ogre::Real a_x, Ogre::Real a_y);

    //check if position is valid
    bool isOutOfBounds(Ogre::Vector3& pos_xyz);
    bool isOutOfArena(Ogre::Vector3& pos_xyz);

    //partition all objects into cells
    void setCellIndex(uint_pair a_cell_index, Corpus* a_thing);
    //returns true if it's out of bounds - possibly confusing but convenient
    bool updateCellIndex(uint_pair& cell_index, Ogre::Vector3& pos_xyz, Mobilis* a_thing);
    bool updateCellIndex(uint_pair& cell_index, Ogre::Vector3& pos_xyz, Unit* a_thing);

    //accessing objects on the map based on the cell index
    list<Corpus*>* getCorpusCell(const uint a_i, const uint a_j);
    list<Mobilis*>* getMobilisCell(const uint a_i, const uint a_j);
    list<Unit*>* getUnitCell(const uint a_i, const uint a_j);
    //fills an array with lists
    void getCorpusCellGrid(const uint_pair cell_index, list<Corpus*>* grid[3]);
    void getMobilisCellGrid(const uint_pair cell_index, list<Mobilis*>* grid[3]);
    void getUnitCellGrid(const uint_pair cell_index, list<Unit*>* grid[3]);

private:
    //inner main loop
    void updateLights();
    void updateCells();

    //gravity
    Ogre::Real gravity;

    //terrain heightmap specs
    unsigned int texture_size;
    Ogre::Real inverse_scale;
    //size of the whole terraun
    Ogre::Real scene_size;

    //terrain graphics
    Ogre::SceneNode* terrain_node;

    //terrain data
    Ogre::Real* height;
    Ogre::Real* angle_x;
    Ogre::Real* angle_y;
    string terrain_name;

    //lights
    Ogre::Light* sunlight;
    Ogre::Light* backlight;
    Ogre::Light* ground_light; //for ambient

    //controllers for the units on the map cotnrolled by ais
    vector<GameController*> ai_game_controllers;

    //all the objects in the arena segregated into cells
    list<Corpus*> corpus_cells[num_of_arena_cells][num_of_arena_cells];
    list<Mobilis*> mobilis_cells[num_of_arena_cells][num_of_arena_cells];
    list<Unit*> unit_cells[num_of_arena_cells][num_of_arena_cells];
    //size of individual cells in metres
    Ogre::Real cell_size;
};

/** @brief returns whether the position is not outside of arena bounds and fixes the position
  */
inline bool Arena::isOutOfBounds(Ogre::Vector3& pos_xyz)
{
    //it's within the limits of the arena
    bool out_of_bounds = false;

    //if it's not, fix the position and mark that it was fixed
    if (pos_xyz.x < cell_size) {
        pos_xyz.x = cell_size;
        out_of_bounds = true;
    } else if (pos_xyz.x > scene_size - cell_size) {
        pos_xyz.x = scene_size - cell_size;
        out_of_bounds = true;
    }

    //same for vertical
    if (pos_xyz.z < cell_size) {
        pos_xyz.z = cell_size;
        out_of_bounds = true;
    } else if (pos_xyz.z > scene_size - cell_size) {
        pos_xyz.z = scene_size - cell_size;
        out_of_bounds = true;
    }

    return out_of_bounds;
}
/** @brief returns whether the position is not outside of arena bounds and fixes the position
  */
inline bool Arena::isOutOfArena(Ogre::Vector3& pos_xyz)
{
    //it's within the limits of the arena
    bool out_of_arena = false;

    //if it's not, fix the position and mark that it was fixed
    if (pos_xyz.x < 0) {
        pos_xyz.x = 0;
        out_of_arena = true;
    } else if (pos_xyz.x > scene_size) {
        pos_xyz.x = scene_size;
        out_of_arena = true;
    }

    //same for vertical
    if (pos_xyz.z < 0) {
        pos_xyz.z = 0;
        out_of_arena = true;
    } else if (pos_xyz.z > scene_size) {
        pos_xyz.z = scene_size;
        out_of_arena = true;
    }

    return out_of_arena;
}

/** @brief returns the index of the cell in the arena the position is in
  */
inline uint_pair Arena::getCellIndex(Ogre::Real a_x, Ogre::Real a_y)
{
    //get the index of the cell pased on position
    unsigned int i = a_x / cell_size;
    unsigned int j = a_y / cell_size;

    return make_pair(i, j);
}

inline list<Corpus*>* Arena::getCorpusCell(const uint a_i, const uint a_j)
{
    return &corpus_cells[a_i][a_j];
}

inline list<Mobilis*>* Arena::getMobilisCell(const uint a_i, const uint a_j)
{
    return &mobilis_cells[a_i][a_j];
}

inline list<Unit*>* Arena::getUnitCell(const uint a_i, const uint a_j)
{
    return &unit_cells[a_i][a_j];
}

#endif // ARENA_H
