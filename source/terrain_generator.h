//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "main.h"

class Terrain;

class TerrainGenerator
{
public:
    ~TerrainGenerator() { };

    static Terrain* generateTerrain(const string& terrain_name);

    //gets the weight for a vertex height for blending tiles together
    static Ogre::Real getBlendWeight(uint a_i, uint a_j);

private:
    TerrainGenerator() { };

};

#endif // TERRAINGENERATOR_H
