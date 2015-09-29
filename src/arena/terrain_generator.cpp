// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game.h"
#include "terrain_generator.h"
#include "files_handler.h"
#include "terrain.h"

const usint block_size = terrain::block_size;
const int block_blend_size = 8; // each side
const int inner_block_size = block_size - 2 * block_blend_size;
// max height needs to be adjusted so that it covers the inner size of the block
const Real effective_height = Real(block_size) / Real(inner_block_size);

/** @brief this gets the deifinition of a premade map of tiles
 */
bool TerrainGenerator::getTerrain(const string&                 filename,
                                  vector<usint>&                block_height,
                                  vector<terrain::block_types>& block_type,
                                  size_t&                       width,
                                  size_t&                       height)
{
  // prepare map to read tile data into
  map<string, string> pairs;
  if (!FilesHandler::getPairs(filename + EXT_TERRAIN, TERRAIN_DIR, pairs)) { // insert data from
    // file into
    // pairs
    return false;
  }

  width = intoInt(pairs["width"]);
  height = intoInt(pairs["height"]);

  vector<char> tile_type_codes;

  FilesHandler::getUsintDigitArray(block_height, pairs["terrain_height"]);
  FilesHandler::getCharArray(tile_type_codes, pairs["terrain_type"]);

  // converting a character code array into an terrain tile type enum array
  for (size_t i = 0, for_size = tile_type_codes.size(); i < for_size; ++i) {
    terrain::block_types type;

    if (tile_type_codes[i] == 'p') {
      type = terrain::block_plain;
    } else if (tile_type_codes[i] == 'h') {
      type = terrain::block_hills;
    } else if (tile_type_codes[i] == 'm') {
      type = terrain::block_mountains;
    } else if (tile_type_codes[i] == 'w') {
      type = terrain::block_cliff;
    } else if (tile_type_codes[i] == 'c') {
      type = terrain::block_coast;
    } else if (tile_type_codes[i] == 'r') {
      type = terrain::block_river;
    }

    block_type.push_back(type);
  }

  return true;
}

/** @brief generate a terrain from a terrain file
 * @todo: pick a random terrain file from a set of ones or randomise the read data a bit
 */
Terrain* TerrainGenerator::generateTerrain(const string& terrain_name)
{
  // this part deals with the big prefab tiles that are blended together to form the map

  // this is the tile data, not actual map data
  vector<usint> block_heights;
  vector<terrain::block_types> block_types;
  size_t tile_columns(0);
  size_t tile_rows(0);

  // load the terrain tile data from a file
  getTerrain(terrain_name, block_heights, block_types, tile_columns, tile_rows);

  // this is the actual per vertex terrain
  Real terrain_w = tile_columns * inner_block_size;
  Real terrain_h = tile_rows * inner_block_size;
  Terrain* result = new Terrain(terrain_w, terrain_h);

  // analyse the hights and convert to a set of tile tile_rows types
  for (size_t i = 0; i < tile_columns; ++i) {
    for (size_t j = 0; j < tile_rows; ++j) {
      // this is the tile we are creating
      size_t index = i + j * tile_columns;

      // read the type and tile_rows for this tile
      const terrain::block_types& block_type = block_types[index];
      const Real& max_height = terrain::max_tile_heights[block_type];

      // the suffix will identify a tile definition, the type of suffix depends on tile type
      string tile_name;

      // we need to find neighouring tiles' tile_rows to determine what tile definition to use
      if (block_type == terrain::block_plain
          || block_type == terrain::block_hills
          || block_type == terrain::block_mountains) {
        // find four neighours of the block to determine the gradient
        int index_adj[4];
        index_adj[0] = index - tile_columns + 1; // top right corner
        index_adj[1] = index + tile_columns + 1; // bottom right
        index_adj[2] = index + tile_columns - 1; // bottom left
        index_adj[3] = index - tile_columns - 1; // top left

        // repeat terrain for the edges
        if (index % tile_columns == tile_columns - 1) {
          // right edge
          index_adj[0] -= 1;
          index_adj[1] -= 1;
        } else if (index % tile_columns == 0) {
          // left edge
          index_adj[2] += 1;
          index_adj[3] += 1;
        }

        if (index < tile_columns) {
          // top edge
          index_adj[0] += tile_columns;
          index_adj[3] += tile_columns;
        } else if (index >= tile_columns * tile_rows - tile_columns) {
          // bottom edge
          index_adj[1] -= tile_columns;
          index_adj[2] -= tile_columns;
        }

        // create a name of the image that defines a tile that fits prefix_0000
        tile_name = terrain::block_prefix[block_type];

        for (size_t i_block = 0; i_block < 4; ++i_block) {
          // ternary operators are neat but also annoying when debugging, so there
          if ((block_heights[index_adj[i_block]] > block_heights[index])) {
            tile_name += '1';

          } else {
            tile_name += '0';
          }
        }
      } else {
        // panic!
        tile_name = terrain::block_prefix[terrain::block_plain] + "0000";
      }

      // TODO:
      // cliff
      // coast
      // river

      // todo: load all definitions in advance and put them in array
      Ogre::Image* tile = new Ogre::Image();
      tile->load(tile_name + ".tga", "terrain");

      // load the data from the image and blend it into the terrain
      for (size_t t_i = 0; t_i < block_size; ++t_i) {
        for (size_t t_j = 0; t_j < block_size; ++t_j) {
          Ogre::ColourValue colour = tile->getColourAt(t_i, t_j, 0);

          // translate local tile coords into global terrain coords
          int ter_x = i * inner_block_size + t_i - block_blend_size;
          int ter_y = j * inner_block_size + t_j - block_blend_size;

          // cut off the bleeds around the terrain as blend weights there will be wrong
          if (ter_x >= 0 && ter_x < terrain_w && ter_y >= 0 && ter_y < terrain_h) {
            // read the height on the block from the image
            Real vertex_h = colour.g * max_height * effective_height;
            // add the height of the whole block in the world
            vertex_h += block_heights[index] * terrain::min_block_height;

            // blend in the height
            Real height_weight = getBlendWeight(t_i, t_j);
            result->blendHeight(ter_x, ter_y, vertex_h, height_weight);
          }
        }
      }

      // this needs an intermediary step for blending types in non trivial way

      // temp types
      for (size_t t_i = 0; t_i < block_size; ++t_i) {
        for (size_t t_j = 0; t_j < block_size; ++t_j) {
          // Ogre::ColourValue colour = tile->getColourAt(t_i, t_j, 0);

          // translate local tile coords into global terrain coords
          int ter_x = i * inner_block_size + t_i - block_blend_size;
          int ter_y = j * inner_block_size + t_j - block_blend_size;

          // cut off the bleeds around the terrain as blend weights there will be wrong
          if (ter_x >= 0 && ter_x < terrain_w && ter_y >= 0 && ter_y < terrain_h) {
            result->setType(ter_x, ter_y, terrain::plain);
          }
        }
      }

    }
  }

  return result;
}

/** @brief gets the weight for a vertex height for blending tiles together
 * @todo: experiment with non-linear blends
 */
Real TerrainGenerator::getBlendWeight(size_t a_i,
                                      size_t a_j)
{
  // get the shorter distance from any edge
  if (a_i > block_size * 0.5) {
    a_i = block_size - a_i;
  }

  if (a_j > block_size * 0.5) {
    a_j = block_size - a_j;
  }

  Real weight_i = Real(a_i) / Real(2 * block_blend_size);
  clampZeroOne(weight_i);

  Real weight_j = Real(a_j) / Real(2 * block_blend_size);
  clampZeroOne(weight_j);

  return (weight_i * weight_j);
}
