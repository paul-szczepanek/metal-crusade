// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef TERRAIN_H
#define TERRAIN_H

#include "main.h"

// 1pixel = 8 metres
const Real metres_per_pixel = 8;

namespace terrain
{
const int block_size = 32; // in pixels

const Real min_block_height = block_size * 0.5;

enum block_types {
  block_plain,
  block_hills,
  block_mountains,
  block_cliff,
  block_coast,
  block_river,
  block_types_max
};

// white colour means 16 * scale metres high
const Real max_tile_heights[block_types_max] = {
  block_size * 0.5, // plain, 26 degrees
  block_size, // hills, 45 d
  block_size * 2, // mountains, 64 d
  block_size, // cliff,
  block_size * 0.5, // coast,
  block_size * 0.5 // river
};

enum types {
  plain,
  hills,
  mountains,
  cliff,
  coast,
  river,
  types_max
};

const string block_prefix[block_types_max] = {
  "ht_p_",
  "ht_p_", // temp should be _h_
  "ht_m_",
  "ht_w_",
  "ht_c_",
  "ht_r_",
};
};

class Terrain
{
public:
  Terrain(size_t a_width,
          size_t a_height);
  ~Terrain()
  {
  }

  // get height of vertex
  Real getHeight(size_t a_x,
                 size_t a_y);
  // get height and angle at arbitrary point, blend nearest vertices
  Real getHeight(Real a_x,
                 Real a_y);
  real_pair getAngle(Real a_x,
                     Real a_y);

  // set height and type at vertex
  void setHeight(size_t a_x,
                 size_t a_y,
                 Real   a_height);
  void blendHeight(size_t a_x,
                   size_t a_y,
                   Real   a_height,
                   Real   a_weight);
  void setType(size_t         a_x,
               size_t         a_y,
               terrain::types a_type);

  size_t size_w;
  size_t size_h;

private:

  Real inverse_scale;
  vector<Real> terrain_height;
  vector<terrain::types> terrain_type;
};

inline Real Terrain::getHeight(size_t a_x,
                               size_t a_y)
{
  return terrain_height[a_x + a_y * size_w];
}

inline void Terrain::setHeight(size_t a_x,
                               size_t a_y,
                               Real   a_height)
{
  terrain_height[a_x + a_y * size_w] = a_height;
}

inline real_pair Terrain::getAngle(Real a_x,
                                   Real a_y)
{
  real_pair angle(0, 0);

  return angle;
}

/** @brief returns the height of the terrain at this point
 */
inline Real Terrain::getHeight(Real a_x,
                               Real a_y)
{
  // translate world coords into texture coords
  Real texture_x = a_x * inverse_scale;
  Real texture_y = a_y * inverse_scale;

  // get upper left corner index of of the sampled square
  size_t i = texture_x;
  size_t j = texture_y;

  // sample four heights
  Real sample1 = terrain_height[i + j * size_w];
  Real sample2 = terrain_height[i + 1 + j * size_w];
  Real sample3 = terrain_height[i + (j + 1) * size_w];
  Real sample4 = terrain_height[i + 1 + (j + 1) * size_w];

  // get x and y in the 1 by 1 pixel size cell
  Real weight_x = texture_x - i;
  Real weight_y = texture_y - j;
  Real weight_x_co1 = 1 - weight_x;
  Real weight_y_co1 = 1 - weight_y;

  // calculate weights for each sample
  Real weight1 = weight_x_co1 * weight_y_co1;
  Real weight2 = weight_x * weight_y_co1;
  Real weight3 = weight_x_co1 * weight_y;
  Real weight4 = weight_x * weight_y;

  // return the sum of weighted samples
  return weight1 * sample1 + weight2 * sample2 + weight3 * sample3 + weight4 * sample4;
}

#endif // TERRAIN_H
