// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "building_factory.h"
#include "corpus.h"
#include "game.h"
#include "game_arena.h"
#include "query_mask.h"
#include "arena_entity.h"

BuildingFactory::~BuildingFactory()
{
  for (list<Corpus*>::iterator it = corpus.begin(), it_end = corpus.end(); it != it_end; ++it) {
    delete *it;
  }

  corpus.clear();
}

/** @brief creates static buildings and add's them to a list - they never get updated
  */
ArenaEntity* BuildingFactory::spawnSceneryBuidling(Ogre::Vector3 a_pos_xyz, const string& a_name,
    Ogre::Quaternion a_orientation)
{
  // get unique string from id and append the name
  string id_string = getUniqueID() + '_' + a_name;
  ArenaEntity Entity(id_string);

  // create entity
  Ogre::Entity* building_mesh = Game::scene->createEntity(id_string, a_name + ".mesh");

  // assign material
  building_mesh->setMaterialName(a_name);
  // set flags
  building_mesh->setQueryFlags(query_mask_scenery);

  // create the root node
  Ogre::SceneNode* building_node = Game::scene->getRootSceneNode()->createChildSceneNode();
  // attach the mesh
  building_node->attachObject(building_mesh);

  // put the structure on the list
  corpus.push_back(new Corpus(a_pos_xyz, a_name, building_node, a_orientation));

  // position the node for the Corpus because it doens't do a whole lot by itself
  building_node->setPosition(a_pos_xyz);

  return corpus.back();
}

/** @brief creates static buildings with automatic height
  */
ArenaEntity* BuildingFactory::spawnSceneryBuidling(Ogre::Real a_x, Ogre::Real a_y,
    const string& a_name,
    Ogre::Quaternion a_orientation)
{
  return spawnSceneryBuidling(Ogre::Vector3(a_x, Game::Arena->getHeight(a_x, a_y), a_y),
                              a_name, a_orientation);
}
