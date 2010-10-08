//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef ARENA_H
#define ARENA_H

#include "main.h"

//the arena is partitioned into cells to help find things and potential collisions
const unsigned int size_of_arena_cell = 64; //size in metres

//the rule is: no dynamic object bounding sphere can be bigger than 32 metres in diameter
//if a structure is bigger it needs to occupy more than one cell at a time

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
    void purgeCellIndex(uint_pair& cell_index, Mobilis* a_thing);
    void purgeCellIndex(uint_pair& cell_index, Unit* a_thing);

    //accessing objects on the map based on the cell index
    list<Corpus*>& getCorpusCell(const uint_pair a_index);
    list<Mobilis*>& getMobilisCell(const uint_pair a_index);
    list<Unit*>& getUnitCell(const uint_pair a_index);
    //fills an array with lists
    void getCellIndicesWithinRadius(const uint_pair a_index, vector<uint_pair>& indices,
                                    const Ogre::Real a_radius = 0);

private:
    //inner main loop
    void updateLights();
    void updateCells();

    //prepares the cells of the arena
    void partitionArena();

    //all the objects in the arena segregated into cells
    vector<vector<list<Corpus*> > > corpus_cells;
    vector<vector<list<Mobilis*> > > mobilis_cells;
    vector<vector<list<Unit*> > > unit_cells;
    //number of cells a side
    uint num_of_arena_cells;

    //a list of non-empty cells
    list<list<Mobilis*>* > live_mobilis_cells;
    list<list<Unit*>* > live_unit_cells;

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
};

/** @brief returns whether the position is not outside of arena bounds and fixes the position
  */
inline bool Arena::isOutOfBounds(Ogre::Vector3& pos_xyz)
{
    //it's within the limits of the arena
    bool out_of_bounds = false;

    //if it's not, fix the position and mark that it was fixed
    if (pos_xyz.x < size_of_arena_cell) {
        pos_xyz.x = size_of_arena_cell;
        out_of_bounds = true;
    } else if (pos_xyz.x > scene_size - size_of_arena_cell) {
        pos_xyz.x = scene_size - size_of_arena_cell;
        out_of_bounds = true;
    }

    //same for vertical
    if (pos_xyz.z < size_of_arena_cell) {
        pos_xyz.z = size_of_arena_cell;
        out_of_bounds = true;
    } else if (pos_xyz.z > scene_size - size_of_arena_cell) {
        pos_xyz.z = scene_size - size_of_arena_cell;
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
    unsigned int i = a_x / size_of_arena_cell;
    unsigned int j = a_y / size_of_arena_cell;

    return make_pair(i, j);
}

inline list<Corpus*>& Arena::getCorpusCell(const uint_pair a_index)
{
    return corpus_cells[a_index.first][a_index.second];
}

inline list<Mobilis*>& Arena::getMobilisCell(const uint_pair a_index)
{
    return mobilis_cells[a_index.first][a_index.second];
}

inline list<Unit*>& Arena::getUnitCell(const uint_pair a_index)
{
    return unit_cells[a_index.first][a_index.second];
}

/** @brief remove object from the cell
  */
inline void Arena::purgeCellIndex(uint_pair& cell_index, Mobilis* a_thing)
{
    mobilis_cells[cell_index.first][cell_index.second].remove(a_thing);
}

inline void Arena::purgeCellIndex(uint_pair& cell_index, Unit* a_thing)
{
    unit_cells[cell_index.first][cell_index.second].remove(a_thing);
}

#endif // ARENA_H
