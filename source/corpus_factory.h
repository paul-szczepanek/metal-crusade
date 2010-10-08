//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef CORPUSFACTORY_H
#define CORPUSFACTORY_H

#include "factory.h"

class Corpus;

class CorpusFactory : public Factory
{
public:
    CorpusFactory() { };
    ~CorpusFactory();

    Corpus* spawnSceneryBuidling(Ogre::Vector3 a_pos_xyz, const string& a_name,
                                 Ogre::Quaternion a_orientation = Ogre::Quaternion(1, 0, 0, 0));

protected:
    list<Corpus*> corpus;
};


#endif // CORPUSFACTORY_H
