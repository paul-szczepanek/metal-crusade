// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CORPUSFACTORY_H
#define CORPUSFACTORY_H

#include "main.h"

class Corpus;
class ArenaEntity;

class BuildingFactory
{
public:
  BuildingFactory();

  ~BuildingFactory();

  ArenaEntity* spawnSceneryBuidling(Vector3 a_pos_xyz,
                                    const string& a_name,
                                    Quaternion a_orientation = Quaternion(1, 0, 0, 0));
  ArenaEntity* spawnSceneryBuidling(Real a_x,
                                    Real a_y,
                                    const string& a_name,
                                    Quaternion a_orientation = Quaternion(1, 0, 0, 0));

protected:
  list<Corpus*> Corpuses;
  list<ArenaEntity*> Buildings;
};

#endif // CORPUSFACTORY_H
