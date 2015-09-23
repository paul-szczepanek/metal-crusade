// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CORPUS_H
#define CORPUS_H

#include "main.h"
#include "sphere.h"
#include "collision_type.h"

class Collision;
class ArenaEntity;

class Corpus
{
public:
  Corpus();
  virtual ~Corpus();

  // main loop
  virtual int update(Real a_dt);

  // position
  Real getX();
  Real getY();
  Real getZ();
  Vector2 getXZ();
  Vector3 getPosition();
  uint_pair getCellIndex();

  // move around
  void setXYZ(Vector3 a_pos_xyz);
  void setVelocity(Vector3 a_velocity);
  Vector3 getVelocity();
  // put it in the cell on the arena
  void updateCellIndex();

  // orientation
  Quaternion getOrientation();
  void setOrientation(Quaternion a_orientation);
  Quaternion getLookingOrientation();
  Vector3 getDirection();

  // model
  void setSceneNode(Ogre::SceneNode* a_scene_node);

  void setOwner(ArenaEntity* a_owner);

  // collision handling
  void collideWith(Corpus* a_thing);
  bool hasCollidedWith(Corpus* a_thing);

  // damage and heat
  Real getSurfaceTemperature();
  void setSurfaceTemperature(Real aSurfaceTemperature);
  Real getBallisticDmg();
  Real getEnergyDmg();
  Real getHeatDmg();

  // collisions
  collision_type getCollisionType();
  Real getPenetration();
  Real getFriction();

  Sphere getBoundingSphere();
  bitset<MAX_NUM_ES> getExclusionSpheres(Sphere& sphere);
  bitset<MAX_NUM_CS> getCollisionSpheres(Sphere& sphere);
  bitset<MAX_NUM_CS> getCollisionSpheres(Sphere&            sphere,
                                         bitset<MAX_NUM_ES> es_bitset);
  bool getCollisionSpheres(const string& filename);
  // read collision spheres for the object TEMP
  void loadCollisionSpheres(const string& aCollisionName);

  // resolve collisions
  bool validateCollision(Corpus* a_colliding_object);
  int handleCollision(Collision* a_collision);
  bool revertMove(Vector3 a_move);
  Real getWeight();

  /*mfd_view::diagram_type getDiagramType() {
     return mfd_view::object;
     }*/

private:
  void displayCollision(bool a_toggle);
  void displayCollisionHit();
  void displayCollisionUpdate();

private:
  ArenaEntity* OwnerEntity;
  // main ogre scene node for the object with synced position
  Ogre::SceneNode* SceneNode;

  vector<Sphere> CollisionSpheres;
  // collision spheres
  vector<Sphere> ExclusionSpheres;
  vector<bitset<MAX_NUM_CS> > Exclusions;
  bool BoundingSphereInvalid;
  bitset<MAX_NUM_ES> ESInvalid; // exclusion spheres that need their position recalculated
  bitset<MAX_NUM_CS> CSInvalid; // same for collision spheres
  Sphere BoundingSphere;
  vector<usint> CSAreas;
  vector<usint> ESAreas;

  // sphere positions relative to main scene node
  Vector3 RelBSPosition;
  vector<Vector3> RelESPositions;
  vector<Vector3> RelCSPositions;

  // position and orientation
  Vector3 PosXyz;
  Quaternion Orientation;
  Vector3 Direction;

  // collision
  collision_type CollisionType;
  Real Penetration;
  Real SurfaceTemperature;
  Real Friction;
  Real Conductivity;
  Real total_weight;

  // index of the cell in the arena the object is in
  uint_pair CellIndex;

  // speed and position
  Vector3 velocity;
  Vector3 move;
  Real angular_momentum;
  Real corrected_velocity_scalar;
  bool out_of_bounds;

  // collision system
  bool registered;
  // if it's been collided with this frame
  bool collided;
  // list of objects collided with this frame
  vector<Corpus*> collided_with;

  // debug
  vector<Ogre::Entity*> DebugCSEntities;
  vector<Ogre::SceneNode*> DebugCSNodes;
  Ogre::Entity* DebugBSEntity;
  Ogre::SceneNode* DebugBSNode;

  bool DisplayCollisionDebug;
};

// damage reporting
Real Corpus::getDamage(usint a_diagram_element) {
  return core_integrity;
}

Real Corpus::getWeight() {
  return total_weight;
}

// position
Real Corpus::getX() {
  return PosXyz.x;
}

Real Corpus::getY() {
  return PosXyz.y;
}

Real Corpus::getZ() {
  return PosXyz.z;
}

Vector2 Corpus::getXZ() {
  return Vector2(PosXyz.x, PosXyz.z);
}

Vector3 Corpus::getPosition() {
  return PosXyz;
}

uint_pair Corpus::getCellIndex() {
  return CellIndex;
}

// move around
void Corpus::setXYZ(Vector3 a_pos_xyz) {
  PosXyz = a_pos_xyz;
}

void Corpus::setVelocity(Vector3 a_velocity) {
  velocity = a_velocity;
}

Vector3 Corpus::getVelocity() {
  return velocity;
}

// orientation
Quaternion Corpus::getOrientation() {
  return Orientation;
}

Quaternion Corpus::getLookingOrientation() {
  return Orientation;
}

Vector3 Corpus::getDirection();

// hud operation
Real Corpus::getSpeed() {
  return corrected_velocity_scalar;
}

// collision handling
void Corpus::collideWith(Corpus* a_thing) {
  collided_with.push_back(a_thing);
}

bool Corpus::hasCollidedWith(Corpus* a_thing);

// damage and heat
Real Corpus::getSurfaceTemperature() {
  return SurfaceTemperature;
}

Real Corpus::getBallisticDmg() {
  return 0;
}

Real Corpus::getEnergyDmg() {
  return 0;
}

Real Corpus::getHeatDmg() {
  return 0;
}

// collisions
collision_type Corpus::getCollisionType() {
  return CollisionType;
}

Real Corpus::getPenetration() {
  return Penetration;
}

Real Corpus::getFriction() {
  return Friction;
}

bool Corpus::validateCollision(Corpus* a_colliding_object) {
  return true;
}

inline void Corpus::setSurfaceTemperature(Real aSurfaceTemperature)
{
  SurfaceTemperature = aSurfaceTemperature;
}

inline bool Corpus::hasCollidedWith(Corpus* a_thing)
{
  if (collided_with.size() > 0) {
    vector<Corpus*>::iterator it = collided_with.begin();
    vector<Corpus*>::iterator it_end = collided_with.end();

    if (find(it, it_end, a_thing) != it_end) {
      return true;
    }
  }

  return false;
}

#endif // CORPUS_H
