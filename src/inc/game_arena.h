// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef ARENA_H
#define ARENA_H

#include "main.h"

// the arena is partitioned into cells to help find things and potential collisions
const Ogre::Real size_of_arena_cell = 64; // size in metres

// the rule is: no dynamic object bounding sphere can be bigger than size_of_arena_cell metres
// in diameter, if a structure is bigger it needs to occupy more than one cell at a time

class Corpus;
class Terrain;

class GameArena
{
public:
  GameArena();
  ~GameArena();

  // init
  int loadArena(const string& arena_name);

  // main loop
  void update(Ogre::Real a_dt);

  // get height at x and y
  Ogre::Real getHeight(Ogre::Real a_x, Ogre::Real a_y);

  // temperature at any point on the map
  Ogre::Real getAmbientTemperature(Ogre::Vector3 a_position);

  // gravity
  Ogre::Real getGravity() { return gravity; };

  // the arena is partitioned into cells - reutrns the cell index
  uint_pair getCellIndex(Ogre::Real a_x, Ogre::Real a_y);

  // check if position is valid
  bool isOutOfBounds(Ogre::Vector3& pos_xyz);
  bool isOutOfArena(Ogre::Vector3& pos_xyz);

  // returns true if it's out of bounds - possibly confusing but convenient
  bool updateCellIndex(uint_pair& cell_index, Ogre::Vector3& pos_xyz, Corpus* a_thing);
  void purgeCellIndex(uint_pair& cell_index, Corpus* a_thing);

  // accessing objects on the map based on the cell index
  list<Corpus*>& getCorpusCell(const uint_pair a_index);

  // fills an array with lists
  void getCellIndexesWithinRadius(const uint_pair a_index, vector<uint_pair>& indexes,
                                  const Ogre::Real a_radius = 0);

private:
  // inner main loop
  void updateLights();
  void updateCells();

  // prepares the cells of the arena
  void partitionArena();

  // creates the mesh for the terrain
  void createTerrainModel();

  // terrain data
  Terrain* terrain;

  // all the objects in the arena segregated into cells
  vector<vector<list<Corpus*> > > corpus_cells;

  // a list of non-empty cells
  list<list<CorpusCorpus*>* > live_corpus_cells;

  // gravity
  Ogre::Real gravity;

  // terrain heightmap specs
  uint texture_size_w;
  uint texture_size_h;
  // size of the whole arena in metres
  Ogre::Real scene_size_w;
  Ogre::Real scene_size_h;
  // number of cells a side
  uint num_of_arena_cells_w;
  uint num_of_arena_cells_h;

  // terrain graphics
  Ogre::SceneNode* terrain_node;

  // lights
  Ogre::Light* sunlight;
  Ogre::Light* backlight;
  Ogre::Light* ground_light; // for ambient
};

/** @brief returns whether the position is not outside of arena bounds and fixes the position
  */
inline bool GameArena::isOutOfBounds(Ogre::Vector3& pos_xyz)
{
  // it's within the limits of the arena
  bool out_of_bounds = false;

  // if it's not, fix the position and mark that it was fixed
  if (pos_xyz.x < size_of_arena_cell) {
    pos_xyz.x = size_of_arena_cell;
    out_of_bounds = true;
  } else if (pos_xyz.x > scene_size_w - size_of_arena_cell) {
    // we subract a tiny amount so when casting to int we get an index within bounds
    pos_xyz.x = scene_size_w - size_of_arena_cell - Ogre::Real(0.0001);
    out_of_bounds = true;
  }

  // same for vertical
  if (pos_xyz.z < size_of_arena_cell) {
    pos_xyz.z = size_of_arena_cell;
    out_of_bounds = true;
  } else if (pos_xyz.z > scene_size_h - size_of_arena_cell) {
    // we subract a tiny amount so when casting to int we get an index within bounds
    pos_xyz.z = scene_size_h - size_of_arena_cell - Ogre::Real(0.0001);
    out_of_bounds = true;
  }

  return out_of_bounds;
}

/** @brief returns whether the position is not outside of arena bounds and fixes the position
  */
inline bool GameArena::isOutOfArena(Ogre::Vector3& pos_xyz)
{
  // it's within the limits of the arena
  bool out_of_arena = false;

  // if it's not, fix the position and mark that it was fixed
  if (pos_xyz.x < 0) {
    pos_xyz.x = 0;
    out_of_arena = true;
  } else if (pos_xyz.x >= scene_size_w) {
    pos_xyz.x = scene_size_w - Ogre::Real(0.0001); // this is so that casting to int works
    out_of_arena = true;
  }

  // same for vertical
  if (pos_xyz.z < 0) {
    pos_xyz.z = 0;
    out_of_arena = true;
  } else if (pos_xyz.z >= scene_size_h) {
    pos_xyz.z = scene_size_h - Ogre::Real(0.0001); // this is so that casting to int works
    out_of_arena = true;
  }

  return out_of_arena;
}

/** @brief returns the index of the cell in the arena the position is in
  */
inline uint_pair GameArena::getCellIndex(Ogre::Real a_x, Ogre::Real a_y)
{
  // get the index of the cell pased on position
  uint i = a_x / size_of_arena_cell;
  uint j = a_y / size_of_arena_cell;

  return make_pair(i, j);
}

inline list<Corpus*>& GameArena::getCorpusCell(const uint_pair a_index)
{
  return corpus_cells[a_index.first][a_index.second];
}

/** @brief remove object from the cell
  */
inline void GameArena::purgeCellIndex(uint_pair& cell_index, Corpus* a_thing)
{
  corpus_cells[cell_index.first][cell_index.second].remove(a_thing);
}

#endif // ARENA_H
