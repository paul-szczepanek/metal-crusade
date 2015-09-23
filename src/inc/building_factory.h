// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CORPUSFACTORY_H
#define CORPUSFACTORY_H

#include "factory.h"

class Corpus;

class BuildingFactory : public Factory
{
public:
  BuildingFactory() { };
  ~BuildingFactory();

  ArenaEntity* spawnSceneryBuidling(Ogre::Vector3 a_pos_xyz, const string& a_name,
                                    Ogre::Quaternion a_orientation = Ogre::Quaternion(1, 0, 0, 0));
  ArenaEntity* spawnSceneryBuidling(Ogre::Real a_x, Ogre::Real a_y, const string& a_name,
                                    Ogre::Quaternion a_orientation = Ogre::Quaternion(1, 0, 0, 0));

protected:
  list<Corpus*> corpus;
};


#endif // CORPUSFACTORY_H