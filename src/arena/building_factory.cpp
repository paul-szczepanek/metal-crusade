// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "building_factory.h"
#include "corpus.h"
#include "game.h"
#include "game_arena.h"
#include "query_mask.h"
#include "arena_entity.h"

BuildingFactory::BuildingFactory()
{
}

BuildingFactory::~BuildingFactory()
{
  for (list<Corpus*>::iterator it = corpus.begin(), it_end = corpus.end(); it != it_end; ++it) {
    delete *it;
  }

  corpus.clear();
}

/** @brief creates static buildings and add's them to a list - they never get updated
 */
ArenaEntity* BuildingFactory::spawnSceneryBuidling(Vector3       a_pos_xyz,
                                                   const string& a_name,
                                                   Quaternion    a_orientation)
{
  // get unique string from id and append the name
  string id_string = Game::getUniqueID() + '_' + a_name;
  ArenaEntity entity(id_string);

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
  // position the node for the Corpus because it doesn't do a whole lot by itself
  building_node->setPosition(a_pos_xyz);

  Corpus* new_corpus = new Corpus(a_pos_xyz, building_node, a_orientation, entity);
  new_corpus->setSurfaceTemperature(Game::Arena->getAmbientTemperature(a_posXyz));
  new_corpus->loadCollisionSpheres(a_name);
  Game::Collision->registerObject(new_corpus);

  // put the structure on the list
  corpus.push_back(new_corpus);

  return new_corpus;
}

/** @brief creates static buildings with automatic height
 */
ArenaEntity* BuildingFactory::spawnSceneryBuidling(Real          a_x,
                                                   Real          a_y,
                                                   const string& a_name,
                                                   Quaternion    a_orientation)
{
  return spawnSceneryBuidling(Vector3(a_x, Game::Arena->getHeight(a_x, a_y), a_y),
                              a_name, a_orientation);
}
