// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FACTORY_H
#define FACTORY_H

#include "main.h"

class Factory
{
public:
  Factory() { };
  virtual ~Factory() { };

  // destroys the scene node, its children and all the enitites as well
  static void destroyModel(Ogre::SceneNode* a_scene_node);

protected:
  // unique id for creation
  static string getUniqueID();
  static ulint id;
};

/** @brief creates a unique ID for Ogre entity and mesh creation
  */
inline string Factory::getUniqueID()
{
  // get unique string from id
  string id_string;
  stringstream stream;
  stream << setfill('0') << setw(10) << ++id; // format id string to 0000000001
  stream >> id_string;
  return id_string;
}

#endif // FACTORY_H
