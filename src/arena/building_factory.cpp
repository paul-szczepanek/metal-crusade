// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "building_factory.h"
#include "corpus.h"
#include "game.h"
#include "game_arena.h"
#include "query_mask.h"
#include "arena_entity.h"
#include "collision_handler.h"

BuildingFactory::BuildingFactory()
{
}

BuildingFactory::~BuildingFactory()
{
  for (auto c : Corpuses) {
    delete c;
  }
  for (auto b : Buildings) {
    delete b;
  }
}

/** @brief creates static buildings and add's them to a list - they never get updated
 */
ArenaEntity* BuildingFactory::spawnSceneryBuidling(Vector3       a_pos_xyz,
                                                   const string& a_name,
                                                   Quaternion    a_orientation)
{
  // get unique string from id and append the name
  string id_string = Game::getUniqueID() + a_name;
  ArenaEntity* entity = new ArenaEntity(id_string);

  // create entity
  Ogre::Entity* building_mesh = Game::Scene->createEntity(id_string, a_name + ".mesh");

  // assign material
  building_mesh->setMaterialName(a_name);
  // set flags
  building_mesh->setQueryFlags(query_mask_scenery);

  // create the root node
  Ogre::SceneNode* building_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  // attach the mesh
  building_node->attachObject(building_mesh);
  // position the node for the Corpus because it doesn't do a whole lot by itself
  building_node->setPosition(a_pos_xyz);

  Corpus* new_corpus = new Corpus();
  new_corpus->XYZ = a_pos_xyz;
  new_corpus->setSceneNode(building_node);
  new_corpus->setOrientation(a_orientation);
  new_corpus->setOwner(entity);
  new_corpus->SurfaceTemperature = Game::Arena->getAmbientTemperature(a_pos_xyz);
  new_corpus->loadCollisionSpheres(a_name);
  Game::Collision->registerObject(new_corpus);

  // put the structure on the list
  Corpuses.push_back(new_corpus);
  Game::Arena->Entities.push_back(entity);
  Buildings.push_back(entity);

  return entity;
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
