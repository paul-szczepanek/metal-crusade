// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "terrain.h"

/** @brief
 */
Terrain::Terrain(size_t a_width,
                 size_t a_height)
  : size_w(a_width), size_h(a_height), terrain_height(a_width * a_height, 0.f),
  terrain_type(a_width * a_height, terrain::plain)
{
  inverse_scale = Real(1) / metres_per_pixel;
}

void Terrain::blendHeight(size_t a_x,
                          size_t a_y,
                          Real   a_height,
                          Real   a_weight)
{
  terrain_height[a_x + a_y * size_w] = terrain_height[a_x + a_y * size_w] + a_height * a_weight;
}

void Terrain::setType(size_t         a_x,
                      size_t         a_y,
                      terrain::types a_type)
{
  terrain_type[a_x + a_y * size_w] = a_type;
}
