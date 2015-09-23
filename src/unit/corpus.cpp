// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "corpus.h"
#include "query_mask.h"
#include "game.h"
#include "files_handler.h"
#include "entity.h"
// #include "game_arena.h"

Corpus::Corpus()
  : Owner(NULL),
  BoundingSphereInvalid(true),
  ESInvalid(0),
  CSInvalid(0),
  PosXyz(a_posXyz),
  SceneNode(NULL),
  CollisionType(collision_type_blocking),
  Penetration(0),
  Friction(0.5),
  Conductivity(1),
  CellIndex(make_pair(0, 0)),
  DisplayCollisionDebug(false),
  velocity(Vector3(0, 0, 0)),
  move(Vector3(0, 0, 0)),
  corrected_velocity_scalar(0),
  out_of_bounds(false),
  registered(false),
  core_integrity(1),
  target(NULL)
{

  //Orientation = scene_node->getOrientation();

  ESInvalid.set();
  CSInvalid.set();
}

void Corpus::setOrientation(Quaternion a_orientation){
  Direction = Orientation * Vector3::UNIT_Z;
}

void Corpus::setSceneNode(Ogre::SceneNode* a_scene_node){
  SceneNode = a_scene_node;
}

void Corpus::setOwner(ArenaEntity* a_owner){
  Owner = a_owner;
}

Corpus::~Corpus()
{
  if (SceneNode) {
    destroyModel(SceneNode);
  }
  clearFromTargets();
}

/** @brief get bounding sphere and update it's position
 */
inline Sphere Corpus::getBoundingSphere()
{
  if (BoundingSphereInvalid) { // if object moved last frame offset sphere
    BoundingSphereInvalid = false;
    BoundingSphere.Centre = RelBSPosition + PosXyz;
  }

  return BoundingSphere;
}

/** @brief get exclusion spheres based on the passed in sphere
 */
bitset<MAX_NUM_ES> Corpus::getExclusionSpheres(Sphere& a_sphere)
{
  // by default return no spheres
  bitset<MAX_NUM_ES> es_bitset;

  for (usint i = 0, for_size = ExclusionSpheres.size(); i < for_size; ++i) {
    // recaltulate position if it's first time this tick
    if (ESInvalid[i]) {
      ExclusionSpheres[i].Centre = Orientation * RelESPositions[i] + PosXyz;

      // mark recalculated
      ESInvalid[i] = 0;
    }

    // if it intersects return as potential exclusion sphere
    if (ExclusionSpheres[i].intersects(a_sphere)) {
      es_bitset[i] = 1;
    }
  }

  return es_bitset;
}

/** @brief get collision spheres without excluding any - just update
 */
bitset<MAX_NUM_CS> Corpus::getCollisionSpheres(Sphere& a_sphere)
{
  bitset<MAX_NUM_ES> es_bitset;

  // assume it intersects with all spheres
  es_bitset.set();

  return getCollisionSpheres(a_sphere, es_bitset);
}

/** @brief gets collision spheres but doesn't check the ones excluded by es_bitset
 *        and updates their positions based on object position
 */
bitset<MAX_NUM_CS> Corpus::getCollisionSpheres(Sphere&            a_sphere,
                                               bitset<MAX_NUM_ES> es_bitset)
{
  bitset<MAX_NUM_CS> cs_bitset;

  // by default return all spherers
  cs_bitset.set();

  // stop this loop if you eliminate all spheres
  for (usint i = 0, for_size = ExclusionSpheres.size(); i < for_size && cs_bitset.any(); ++i) {
    // only check potential exclusion spheres
    if (es_bitset[i]) {
      // recaltulate position if it's first time this tick
      if (ESInvalid[i]) {
        // mark recalculated
        ESInvalid[i] = 0;

        // reaclucalte position depenting on current unit position and Orientation
        ExclusionSpheres[i].Centre = Orientation * RelESPositions[i]
                                     + PosXyz;
      }

      // if it doesn't intersect with the exclusion sphere
      if (ExclusionSpheres[i].intersects(a_sphere) == false) {
        // exclude the all spheres within the exclusion sphere
        cs_bitset &= Exclusions[i];
      }
    } else {
      // if it's already been checked to be outside - exclude
      cs_bitset &= Exclusions[i];
    }
  }

  // check if it's returning anything at all
  if (cs_bitset.any()) {
    // make sure the spheres the indexes we return of are in an up to date position
    for (usint i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
      // recalculate position if it's first time this tick
      if (cs_bitset[i] && CSInvalid[i]) {
        // mark recalculated
        CSInvalid[i] = 0;

        // reaclucalte position depenting on current unit position and Orientation
        CollisionSpheres[i].Centre = Orientation * RelCSPositions[i] + PosXyz;
      }
    }
  }

  return cs_bitset;
}

/** @brief loads collision spheres from a file based on mesh name
 */
void Corpus::loadCollisionSpheres(const string& a_collision_name)
{
  // TEMP!!! fake, only works for crusaders for now
  if (a_collision_name == "bullet") {
    RelBSPosition = Vector3::ZERO;
    BoundingSphere = Sphere(RelBSPosition, 2);

  } else {
    if (getCollisionSpheres(a_collision_name + "/" + a_collision_name + "_collision")) {

      // store the initial positions of exclusion spheres as relative positions
      for (usint i = 0, for_size = ExclusionSpheres.size(); i < for_size; ++i) {
        RelESPositions.push_back(ExclusionSpheres[i].Centre);
      }
      // same for collision spheres
      for (usint i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
        RelCSPositions.push_back(CollisionSpheres[i].Centre);
      }
      // and the bounding sphere
      RelBSPosition = BoundingSphere.Centre;

    } else {
      Game::kill(a_collision_name + " collision file is corrupt, we can't play the game like this");
    }
  }

  // debug
  // displayCollision(true);
}

/** @brief loads collision spheres from a file
 */
bool Corpus::getCollisionSpheres(const string& a_filename)
{
  map<string, string> pairs;
  assert(FS::getPairs(a_filename, MODEL_DIR, pairs));

  // bounding sphere
  vector<string> bs_sphere_string;
  FS::getStringArray(bs_sphere_string, pairs["bs"]);

  // makes sure the are enough values
  if (bs_sphere_string.size() < 4) {
    cout << "bounding sphere sphere in ";
    return false;
  }

  // read in the bounding sphere
  BoundingSphere.Centre = Vector3(FS::getReal(bs_sphere_string[0]),
                                  FS::getReal(bs_sphere_string[1]),
                                  FS::getReal(bs_sphere_string[2]));
  BoundingSphere.Radius = FS::getReal(bs_sphere_string[3]);

  Sphere sphere;
  int i = 0;

  // check to see if a exclusion sphere with a consecutive id exists
  while (pairs.find(string("es.") + intoString(i)) != pairs.end() && i < MAX_NUM_ES) {
    // load the es definition string
    vector<string> es_sphere_string;
    FS::getStringArray(es_sphere_string, pairs["es." + intoString(i)]);

    // makes sure the are enough values
    if (es_sphere_string.size() < 5) {
      cout << "exclusion sphere sphere in ";
      return false;
    }

    // exclusion sphere area (to move the sphere when it moves)
    ESAreas.push_back(intoInt(es_sphere_string[1]));

    // read in the exclusion sphere
    sphere.Centre = Vector3(FS::getReal(es_sphere_string[1]),
                            FS::getReal(es_sphere_string[2]),
                            FS::getReal(es_sphere_string[3]));
    sphere.Radius = FS::getReal(es_sphere_string[4]);
    // put the sphere in the vector
    ExclusionSpheres.push_back(sphere);

    // create an empty bitset for the sphere
    Exclusions.push_back(bitset<MAX_NUM_CS>(0));

    ++i;
  }

  i = 0;

  // check to see if a collision sphere with a consecutive id exists
  while (pairs.find(string("cs.") + intoString(i)) != pairs.end() && i < MAX_NUM_CS) {
    // load the cs definition string
    vector<string> cs_sphere_string;
    FS::getStringArray(cs_sphere_string, pairs["cs." + intoString(i)]);

    // makes sure the are enough values
    if (cs_sphere_string.size() < 6) {
      cout << "collision sphere in ";
      return false;
    }

    // collision sphere area (to place hits)
    CSAreas.push_back(intoInt(cs_sphere_string[0]));

    // set exclusion spheres' bitsets
    for (usint j = 0, for_size = Exclusions.size(); j < for_size; ++j) {
      // if the bit is set for this es then set the bit for the cs in the es bitset
      if (cs_sphere_string[1][j] == '1') {
        Exclusions[j][i] = 1;
      }
    }

    // read in the collision sphere
    sphere.Centre = Vector3(FS::getReal(cs_sphere_string[2]),
                            FS::getReal(cs_sphere_string[3]),
                            FS::getReal(cs_sphere_string[4]));
    sphere.Radius = FS::getReal(cs_sphere_string[5]);
    // put the sphere in the vector
    CollisionSpheres.push_back(sphere);

    ++i;
  }

  return true;
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
    DebugBSNode = Game::OgreScene->getRootSceneNode()->createChildSceneNode();
    DebugBSEntity = Game::OgreScene->createEntity(id_string + "_debug_sphere",
                                                  "sphere2.mesh");
    DebugBSEntity->setCastShadows(false);

    // attach meshes
    DebugBSNode->attachObject(DebugBSEntity);
    DebugBSEntity->setQueryFlags(query_mask_ignore);
    DebugBSNode->setScale(BoundingSphere.Radius, BoundingSphere.Radius,
                          BoundingSphere.Radius);

    for (usint i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
      id_string = Game::getUniqueID();
      DebugCSNodes.push_back(Game::OgreScene->getRootSceneNode()->createChildSceneNode());
      DebugCSEntities.push_back(Game::OgreScene->createEntity(id_string + "_debug_sphere",
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
    Game::OgreScene->destroySceneNode(DebugBSNode);

    for (usint i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
      DebugCSNodes.back()->getCreator()->destroyMovableObject(DebugCSEntities[i]);
      Game::OgreScene->destroySceneNode(DebugCSNodes[i]);
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
  for (usint i = 0, for_size = CollisionSpheres.size(); i < for_size; ++i) {
    DebugCSNodes[i]->setPosition(CollisionSpheres[i].Centre);
  }
}

Vector3 Corpus::getDirection()
{
  return Direction;
}

/** @brief handleCollision
 * @todo: use this to react graphically to a collision
 */
int Corpus::handleCollision(Collision* a_collision)
{
  scene_node->showBoundingBox(true); // temp

  return 0;
}

/** @brief called by an object which holds this as a target to tell it that it no longer targets it
 */
void Corpus::releaseAsTarget(Corpus* a_targeted_by)
{
  if (target_holders.size() > 0) {
    vector<Corpus*>::iterator it = find(target_holders.begin(),
                                        target_holders.end(), a_targeted_by);
    if (it < target_holders.end()) {
      target_holders.erase(it);
    }
  }
}

/** @brief called by other object to try and acquire this as a target
 * return false if target can't be acquired
 */
bool Corpus::acquireAsTarget(Corpus* a_targeted_by)
{
  target_holders.push_back(a_targeted_by);

  return true;
}

/** @brief reverts an illegal move
 * @todo: replace by something less painfully simplistic
 */
bool Corpus::revertMove(Vector3 a_move)
{
  pos_xyz -= a_move; // haha only serious

  return true;
}

/** @brief game logic, physics and control
 */
int Corpus::update(Real a_dt)
{
  Dt = a_dt;
  updateCellIndex();

  // clear last frame collisions
  collided_with.clear();

  // chain the corpus update
  int return_code = 0;

  // update the scene node
  scene_node->setPosition(pos_xyz);
  scene_node->setOrientation(orientation);

  // mark collision spheres as moved
  bounding_sphere_invalid = true;
  es_invalid.set();
  cs_invalid.set();

  // collision system hookup
  if (return_code == 0) { // if it's not dead
    if (!registered) { // and not registered yet
      registered = true; // register it with the collision system
      Game::collider->registerObject(this);
    }
  } else { // if it's dead
    if (registered) { // remove from collisin checking
      Game::collider->deregisterObject(this);
      Game::Arena->purgeCellIndex(cell_index, this);
    }
  }

  // temp debug
  if (DisplayCollisionDebug) {
    displayCollisionUpdate();
  }

  return return_code;
}

/** @brief puts the object on the arean and in the correct array and in the correct cell index
 * updates the cell index and position if out of bounds
 * by the magic of virtual tables it knows which array to use
 */
void Corpus::updateCellIndex()
{
  // check the position in the arena and update the arena cells if necessary
  out_of_bounds = Game::Arena->updateCellIndex(cell_index, pos_xyz, this);
}
