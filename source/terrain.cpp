//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "terrain.h"

/** @brief
  */
Terrain::Terrain(uint a_width, uint a_height)
    : size_w(a_width), size_h(a_height), terrain_height(a_width * a_height, 0.f),
    terrain_type(a_width * a_height, terrain::plain)
{
    inverse_scale = Ogre::Real(1) / metres_per_pixel;
}

void Terrain::blendHeight(uint a_x, uint a_y, Ogre::Real a_height, Ogre::Real a_weight)
{
    terrain_height[a_x + a_y * size_w] = terrain_height[a_x + a_y * size_w] + a_height * a_weight;
}
