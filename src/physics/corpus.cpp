// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "corpus.h"
#include "query_mask.h"
#include "game.h"
#include "files_handler.h"
#include "arena_entity.h"
#include "game_arena.h"
#include "corpus_manager.h"
#include "collision.h"

Corpus::Corpus(ArenaEntity*     a_owner,
               Ogre::SceneNode* a_scene_node)
  : OwnerEntity(a_owner),
  SceneNode(a_scene_node),
  CollisionType(collision_type_blocking)
{
}

void Corpus::setSceneNode(Ogre::SceneNode* a_scene_node)
{
  SceneNode = a_scene_node;
  //Orientation = SceneNode->getOrientation();
}

void Corpus::setOwner(ArenaEntity* a_owner)
{
  OwnerEntity = a_owner;
  XYZ = OwnerEntity->getXYZ();
  invalidateSpheres();
}

Corpus::~Corpus()
{
  if (SceneNode) {
    Game::destroyModel(SceneNode);
  }

  Game::Corpus->deregisterObject(this);
}

/** @brief get bounding sphere and update it's position
 */
void Corpus::updateBoundingSphere()
{
  if (BSInvalid) { // if object moved last frame offset sphere
    BSInvalid = false;
    BoundingSphere.Centre = Orientation * RelBSPosition + XYZ;
  }
}

/** @brief check spheres against passed in bounding sphere to eliminate obvious ones
 */
void Corpus::pruneCollisionSpheres(const Sphere&       a_sphere,
                                   bitset<MAX_NUM_CS>& a_cs_bitset)
{
  for (size_t i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
    // recaltulate position if it's first time this tick
    if (CSInvalid) {
      CollisionSpheres[i].Centre = Orientation * RelCSPositions[i] + XYZ;
    }

    a_cs_bitset[i] = CollisionSpheres[i].intersects(a_sphere);
  }

  // mark recalculated
  CSInvalid = false;
}

/** @brief loads collision spheres from a file based on mesh name
 */
void Corpus::loadCollision(const string& a_collision_name)
{
  // TEMP!!! fake, only works for crusaders for now
  if (a_collision_name == "bullet") {
    RelBSPosition = Vector3::ZERO;
    BoundingSphere = Sphere(RelBSPosition, 2);
    CollisionSpheres.push_back(Sphere(RelBSPosition, 2));
    RelCSPositions.push_back(Vector3::ZERO);
    CSAreas.push_back(0);
    return;
  } else if (a_collision_name == "ground") {
    RelBSPosition = Vector3::ZERO;
    BoundingSphere = Sphere(RelBSPosition, 10);
    CollisionSpheres.push_back(Sphere(RelBSPosition, 10));
    RelCSPositions.push_back(Vector3::ZERO);
    CSAreas.push_back(0);
    return;
  }

  string a_filename = a_collision_name + "_collision";

  map<string, string> pairs;
  assert(FilesHandler::getPairs(a_filename, COLLISION_DIR, pairs));

  // bounding sphere
  vector<string> bs_sphere_string;
  FilesHandler::getStringArray(bs_sphere_string, pairs["bs"]);

  // makes sure the are enough values
  if (bs_sphere_string.size() < 4) {
    Game::kill(a_collision_name + " collision file is corrupt, missing bounding sphere");
  }

  // read in the bounding sphere
  BoundingSphere.Centre = Vector3(FilesHandler::getReal(bs_sphere_string[0]),
                                  FilesHandler::getReal(bs_sphere_string[1]),
                                  FilesHandler::getReal(bs_sphere_string[2]));
  BoundingSphere.Radius = FilesHandler::getReal(bs_sphere_string[3]);

  Sphere sphere;
  int i = 0;

  // check to see if a collision sphere with a consecutive id exists
  while (pairs.find(string("cs.") + intoString(i)) != pairs.end() && i < MAX_NUM_CS) {
    // load the cs definition string
    vector<string> cs_sphere_string;
    FilesHandler::getStringArray(cs_sphere_string, pairs["cs." + intoString(i)]);

    // makes sure the are enough values
    if (cs_sphere_string.size() < 5) {
      Game::kill(a_collision_name + " collision file is corrupt, collision sphere garbled");
    }

    // collision sphere area (to place hits)
    CSAreas.push_back(intoInt(cs_sphere_string[0]));

    // read in the collision sphere
    sphere.Centre = Vector3(FilesHandler::getReal(cs_sphere_string[1]),
                            FilesHandler::getReal(cs_sphere_string[2]),
                            FilesHandler::getReal(cs_sphere_string[3]));
    sphere.Radius = FilesHandler::getReal(cs_sphere_string[4]);
    // put the sphere in the vector
    CollisionSpheres.push_back(sphere);

    ++i;
  }

  // same for collision spheres
  for (size_t i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
    RelCSPositions.push_back(CollisionSpheres[i].Centre);
  }
  // and the bounding sphere
  RelBSPosition = BoundingSphere.Centre;

  // debug
  displayCollision(true);
}

/** @brief debug spheres create and destroy
 * not safe to call false unless you call true first
 */
void Corpus::displayCollision(bool a_toggle)
{
  DisplayCollisionDebug = a_toggle;

  if (DisplayCollisionDebug) {
    string id_string = Game::getUniqueID();
    // bounding sphere
    DebugBSNode = Game::Scene->getRootSceneNode()->createChildSceneNode();
    DebugBSEntity = Game::Scene->createEntity(id_string + "_debug_sphere",
                                              "sphere2.mesh");
    DebugBSEntity->setCastShadows(false);

    // attach meshes
    DebugBSNode->attachObject(DebugBSEntity);
    DebugBSEntity->setQueryFlags(query_mask_ignore);
    DebugBSNode->setScale(BoundingSphere.Radius, BoundingSphere.Radius,
                          BoundingSphere.Radius);

    for (size_t i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
      id_string = Game::getUniqueID();
      DebugCSNodes.push_back(Game::Scene->getRootSceneNode()->createChildSceneNode());
      DebugCSEntities.push_back(Game::Scene->createEntity(id_string + "_debug_sphere",
                                                          "sphere.mesh"));
      DebugCSEntities.back()->setCastShadows(false);
      DebugCSNodes.back()->attachObject(DebugCSEntities.back()); // attach meshes
      DebugCSEntities.back()->setQueryFlags(query_mask_ignore);
      DebugCSNodes.back()->setScale(CollisionSpheres[0].Radius,
                                    CollisionSpheres[0].Radius,
                                    CollisionSpheres[0].Radius);
    }
  } else {
    // destroy attached entities
    DebugBSNode->getCreator()->destroyMovableObject(DebugBSEntity);
    Game::Scene->destroySceneNode(DebugBSNode);

    for (size_t i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
      DebugCSNodes.back()->getCreator()->destroyMovableObject(DebugCSEntities[i]);
      Game::Scene->destroySceneNode(DebugCSNodes[i]);
    }

    DebugCSNodes.clear();
    DebugCSEntities.clear();
  }
}

/** @brief debug spheres update to current positions
 * as above - don't use unless spheres exist
 */
void Corpus::displayCollisionUpdate()
{
  DebugBSNode->setPosition(BoundingSphere.Centre);
  for (size_t i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
    DebugCSNodes[i]->setPosition(CollisionSpheres[i].Centre);
  }
}

bool Corpus::validateCollision(Corpus* a_object)
{
  bool valid = (OwnerEntity != a_object->OwnerEntity);
  valid &= (CollisionType != collision_type_blocking
            || a_object->CollisionType != collision_type_blocking);

  return valid;
}

/** @brief handleCollision
 * @todo: use this to react graphically to a collision
 */
bool Corpus::handleCollision(Collision* a_collision)
{
  SceneNode->showBoundingBox(true); // temp

  return true;
}

/** @brief reverts an illegal move
 * @todo: replace by something less painfully simplistic
 */
bool Corpus::revertMove(Vector3 a_move)
{
  XYZ -= a_move; // haha only serious

  return true;
}

/** @brief game logic, physics and control
 */
bool Corpus::update(const Real a_dt)
{
  //if (OldVelocity != Vector3::ZERO) {
    if (OnArena) {
      updateCellIndex();
      invalidateSpheres();
    }

    // update the scene node
    SceneNode->setPosition(XYZ);
    SceneNode->setOrientation(Orientation);
  //}

  // temp debug
  if (DisplayCollisionDebug) {
    displayCollisionUpdate();
  }

  return true;
}

/** @brief puts the object on the arena and in the correct array and in the correct cell index
 * updates the cell index and position if out of bounds
 */
void Corpus::updateCellIndex()
{
  // check the position in the arena and update the arena cells if necessary
  Game::Arena->updateCellIndex(this);
}
