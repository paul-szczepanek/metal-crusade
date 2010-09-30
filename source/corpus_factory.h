//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef CORPUSFACTORY_H
#define CORPUSFACTORY_H

#include "main.h"

class CorpusFactory
{
public:
    CorpusFactory() { };
    virtual ~CorpusFactory() { };

    //destroys the scene node, its children and all the enitites as well
    static void destroyModel(Ogre::SceneNode* a_scene_node);

protected:
    //unique id for creation
    static string getUniqueID();
    static ulint id;
};

/** @brief creates a unique ID for Ogre entity and mesh creation
  */
inline string CorpusFactory::getUniqueID()
{
    //get unique string from id
    string id_string;
    stringstream stream;
    stream << setfill('0') << setw(10) << ++id; //format id string to 0000000001
    stream >> id_string;
    return id_string;
}

#endif // CORPUSFACTORY_H
