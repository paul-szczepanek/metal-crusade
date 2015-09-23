// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FACTORY_H
#define FACTORY_H

#include "main.h"

class Factory
{
public:
  Factory();
  virtual ~Factory();

  // destroys the scene node, its children and all the enitites as well
  static void destroyModel(Ogre::SceneNode* a_scene_node);
};

#endif // FACTORY_H
