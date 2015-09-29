// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef ARENA_H
#define ARENA_H

#include "main.h"

// the arena is partitioned into cells to help find things and potential collisions
const Real size_of_arena_cell = 64; // size in metres

// the rule is: no dynamic object bounding sphere can be bigger than size_of_arena_cell metres
// in diameter, if a structure is bigger it needs to occupy more than one cell at a time

class Corpus;
class Terrain;
class ArenaEntity;

class GameArena
{
public:
  GameArena();
  ~GameArena();

  // init
  int loadArena(const string& arena_name);

  bool update(Real a_dt);

  // get height at x and y
  Real getHeight(Real a_x,
                 Real a_y);

  // temperature at any point on the map
  Real getAmbientTemperature(Vector3 a_position);

  // gravity
  Real getGravity()
  {
    return gravity;
  }

  // the arena is partitioned into cells - reutrns the cell index
  size_t_pair getCellIndex(Real a_x,
                           Real a_y);

  // check if position is valid
  bool isOutOfBounds(Vector3& pos_xyz);
  bool isOutOfArena(Vector3& pos_xyz);

  // returns true if it's out of bounds - possibly confusing but convenient
  bool updateCellIndex(Corpus* a_thing);
  void purgeCellIndex(size_t_pair& cell_index,
                      Corpus*      a_thing);

  // accessing objects on the map based on the cell index
  list<Corpus*>& getCorpusCell(const size_t_pair a_index);

  // fills an array with lists
  void getCellIndexesWithinRadius(const size_t_pair    a_index,
                                  vector<size_t_pair>& indexes,
                                  const Real           a_radius = 0);

private:
  // inner main loop
  void updateLights();
  void updateCells();

  // prepares the cells of the arena
  void partitionArena();

  // creates the mesh for the terrain
  void createTerrainModel();

public:
  // terrain data
  Terrain* terrain;

  // all the objects in the arena segregated into cells
  vector<vector<list<Corpus*> > > corpus_cells;

  // a list of non-empty cells
  list<list<Corpus*>* > LiveCorpusCells;

  vector<ArenaEntity*> Entities;

  // gravity
  Real gravity;

  // terrain heightmap specs
  size_t texture_size_w;
  size_t texture_size_h;
  // size of the whole arena in metres
  Real scene_size_w;
  Real scene_size_h;
  // number of cells a side
  size_t num_of_arena_cells_w;
  size_t num_of_arena_cells_h;

  // terrain graphics
  Ogre::SceneNode* terrain_node;

  // lights
  Ogre::Light* sunlight;
  Ogre::Light* backlight;
  Ogre::Light* ground_light; // for ambient
};

/** @brief returns whether the position is not outside of arena bounds and fixes the position
 */
inline bool GameArena::isOutOfBounds(Vector3& pos_xyz)
{
  // it's within the limits of the arena
  bool out_of_bounds = false;

  // if it's not, fix the position and mark that it was fixed
  if (pos_xyz.x < size_of_arena_cell) {
    pos_xyz.x = size_of_arena_cell;
    out_of_bounds = true;
  } else if (pos_xyz.x > scene_size_w - size_of_arena_cell) {
    // we subract a tiny amount so when casting to int we get an index within bounds
    pos_xyz.x = scene_size_w - size_of_arena_cell - Real(0.0001);
    out_of_bounds = true;
  }

  // same for vertical
  if (pos_xyz.z < size_of_arena_cell) {
    pos_xyz.z = size_of_arena_cell;
    out_of_bounds = true;
  } else if (pos_xyz.z > scene_size_h - size_of_arena_cell) {
    // we subract a tiny amount so when casting to int we get an index within bounds
    pos_xyz.z = scene_size_h - size_of_arena_cell - Real(0.0001);
    out_of_bounds = true;
  }

  return out_of_bounds;
}

/** @brief returns whether the position is not outside of arena bounds and fixes the position
 */
inline bool GameArena::isOutOfArena(Vector3& pos_xyz)
{
  // it's within the limits of the arena
  bool out_of_arena = false;

  // if it's not, fix the position and mark that it was fixed
  if (pos_xyz.x < 0) {
    pos_xyz.x = 0;
    out_of_arena = true;
  } else if (pos_xyz.x >= scene_size_w) {
    pos_xyz.x = scene_size_w - Real(0.0001); // this is so that casting to int works
    out_of_arena = true;
  }

  // same for vertical
  if (pos_xyz.z < 0) {
    pos_xyz.z = 0;
    out_of_arena = true;
  } else if (pos_xyz.z >= scene_size_h) {
    pos_xyz.z = scene_size_h - Real(0.0001); // this is so that casting to int works
    out_of_arena = true;
  }

  return out_of_arena;
}

/** @brief returns the index of the cell in the arena the position is in
 */
inline size_t_pair GameArena::getCellIndex(Real a_x,
                                           Real a_y)
{
  // get the index of the cell pased on position
  size_t i = a_x / size_of_arena_cell;
  size_t j = a_y / size_of_arena_cell;

  return make_pair(i, j);
}

inline list<Corpus*>& GameArena::getCorpusCell(const size_t_pair a_index)
{
  return corpus_cells[a_index.first][a_index.second];
}

/** @brief remove object from the cell
 */
inline void GameArena::purgeCellIndex(size_t_pair& cell_index,
                                      Corpus*      a_thing)
{
  corpus_cells[cell_index.first][cell_index.second].remove(a_thing);
}

#endif // ARENA_H
