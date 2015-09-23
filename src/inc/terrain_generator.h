// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "main.h"
#include "terrain.h"

class Terrain;

class TerrainGenerator
{
public:
  ~TerrainGenerator() {
  }

  static Terrain* generateTerrain(const string& terrain_name);
  static bool getTerrain(const string&                 filename,
                         vector<usint>&                block_height,
                         vector<terrain::block_types>& block_type,
                         uint&                         width,
                         uint&                         height);

  // gets the weight for a vertex height for blending tiles together
  static Real getBlendWeight(uint a_i,
                             uint a_j);

private:
  TerrainGenerator() {
  }

};

#endif // TERRAINGENERATOR_H
