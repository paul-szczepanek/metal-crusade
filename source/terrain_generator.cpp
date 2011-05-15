//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "game.h"
#include "terrain_generator.h"
#include "files_handler.h"
#include "terrain.h"

const usint block_size = terrain::block_size;
const int block_blend_size = 8; //each side
const int inner_block_size = block_size - 2 * block_blend_size;
//max height needs to be adjusted so that it covers the inner size of the block
const Ogre::Real effective_height = Ogre::Real(block_size) / Ogre::Real(inner_block_size);

/** @brief generate a terrain from a terrain file
  * @todo: pick a random terrain file from a set of ones or randomise the read data a bit
  */
Terrain* TerrainGenerator::generateTerrain(const string& terrain_name)
{
    //this part deals with the big prefab tiles that are blended together to form the map

    //this is the tile data, not actual map data
    vector<usint> block_heights;
    vector<terrain::block_types> block_types;
    uint tile_columns(0);
    uint tile_rows(0);

    //load the terrain tile data from a file
    FilesHandler::getTerrain(terrain_name, block_heights, block_types, tile_columns, tile_rows);

    //this is the actual per vertex terrain
    Ogre::Real terrain_w = tile_columns * inner_block_size;
    Ogre::Real terrain_h = tile_rows * inner_block_size;
    Terrain* result = new Terrain(terrain_w, terrain_h);

    //analyse the hights and convert to a set of tile tile_rows types
    for (uint i = 0; i < tile_columns; ++i) {
        for (uint j = 0; j < tile_rows; ++j) {
            //this is the tile we are creating
            uint index = i + j * tile_columns;

            //read the type and tile_rows for this tile
            const terrain::block_types& block_type = block_types[index];
            const Ogre::Real& max_height = terrain::max_tile_heights[block_type];

            //the suffix will identify a tile definition, the type of suffix depends on tile type
            string tile_name;

            //we need to find neighouring tiles' tile_rows to determine what tile definition to use
            if (block_type == terrain::block_plain
                || block_type == terrain::block_hills
                || block_type == terrain::block_mountains) {
                //find four neighours of the block to determine the gradient
                int index_adj[4];
                index_adj[0] = index - tile_columns + 1; //top right corner
                index_adj[1] = index + tile_columns + 1; //bottom right
                index_adj[2] = index + tile_columns - 1; //bottom left
                index_adj[3] = index - tile_columns - 1; //top left

                //repeat terrain for the edges
                if (index % tile_columns == tile_columns - 1) {
                    //right edge
                    index_adj[0] -= 1;
                    index_adj[1] -= 1;
                } else if (index % tile_columns == 0) {
                    //left edge
                    index_adj[2] += 1;
                    index_adj[3] += 1;
                }

                if (index < tile_columns) {
                    //top edge
                    index_adj[0] += tile_columns;
                    index_adj[3] += tile_columns;
                } else if (index >= tile_columns * tile_rows - tile_columns) {
                    //bottom edge
                    index_adj[1] -= tile_columns;
                    index_adj[2] -= tile_columns;
                }

                //create a name of the image that defines a tile that fits prefix_0000
                tile_name = terrain::block_prefix[block_type];

                for (uint i_block = 0; i_block < 4; ++i_block) {
                    //ternary operators are neat but also annoying when debugging, so there
                    if ((block_heights[index_adj[i_block]] > block_heights[index])) {
                        tile_name += '1';

                    } else {
                        tile_name += '0';
                    }
                }
            } else {
                //panic!
                tile_name = terrain::block_prefix[terrain::block_plain]+"0000";
            }

            //TODO:
            //cliff
            //coast
            //river

            //todo: load all definitions in advance and put them in array
            Ogre::Image* tile = new Ogre::Image();
            tile->load(tile_name+".tga", "terrain");

            //load the data from the image and blend it into the terrain
            for (uint t_i = 0; t_i < block_size; ++t_i) {
                for (uint t_j = 0; t_j < block_size; ++t_j) {
                    Ogre::ColourValue colour = tile->getColourAt(t_i, t_j, 0);

                    //translate local tile coords into global terrain coords
                    int ter_x = i * inner_block_size + t_i - block_blend_size;
                    int ter_y = j * inner_block_size + t_j - block_blend_size;

                    //cut off the bleeds around the terrain as blend weights there will be wrong
                    if (ter_x >= 0 && ter_x < terrain_w && ter_y >= 0 && ter_y < terrain_h) {
                        //read the height on the block from the image
                        Ogre::Real vertex_h = colour.g * max_height * effective_height;
                        //add the height of the whole block in the world
                        vertex_h += block_heights[index] * terrain::min_block_height;

                        //blend in the height
                        Ogre::Real height_weight = getBlendWeight(t_i, t_j);
                        result->blendHeight(ter_x, ter_y, vertex_h, height_weight);
                    }
                }
            }

            //this needs an intermediary step for blending types in non trivial way

            //temp types
            for (uint t_i = 0; t_i < block_size; ++t_i) {
                for (uint t_j = 0; t_j < block_size; ++t_j) {
                    //Ogre::ColourValue colour = tile->getColourAt(t_i, t_j, 0);

                    //translate local tile coords into global terrain coords
                    int ter_x = i * inner_block_size + t_i - block_blend_size;
                    int ter_y = j * inner_block_size + t_j - block_blend_size;

                    //cut off the bleeds around the terrain as blend weights there will be wrong
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
Ogre::Real TerrainGenerator::getBlendWeight(uint a_i, uint a_j)
{
    //get the shorter distance from any edge
    if (a_i > block_size * 0.5) {
        a_i = block_size - a_i;
    }

    if (a_j > block_size * 0.5) {
        a_j = block_size - a_j;
    }

    Ogre::Real weight_i = Ogre::Real(a_i) / Ogre::Real(2*block_blend_size);
    clampZeroOne(weight_i);

    Ogre::Real weight_j = Ogre::Real(a_j) / Ogre::Real(2*block_blend_size);
    clampZeroOne(weight_j);

    return (weight_i * weight_j);
}
