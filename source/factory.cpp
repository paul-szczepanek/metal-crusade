//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "factory.h"
#include "game.h"

ulint Factory::id = 0; //unieque id for each object in the game

/** @brief clears the Ogre objects for a projectile
  */
void Factory::destroyModel(Ogre::SceneNode* a_scene_node)
{
    //destroy attached entities
    Ogre::SceneNode::ObjectIterator it = a_scene_node->getAttachedObjectIterator();
    while (it.hasMoreElements()) {
        Ogre::MovableObject* movable_object = static_cast<Ogre::MovableObject*>(it.getNext());
        a_scene_node->getCreator()->destroyMovableObject(movable_object);
    }

    //destroy children if any
    Ogre::SceneNode::ChildNodeIterator it2 = a_scene_node->getChildIterator();
    while (it2.hasMoreElements()) {
        Ogre::SceneNode* child_node = static_cast<Ogre::SceneNode*>(it2.getNext());
        destroyModel(child_node);
    }

    //at last remove the scene node
    Game::scene->destroySceneNode(a_scene_node);
}
