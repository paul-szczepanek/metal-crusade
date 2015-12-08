// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CORPUS_H
#define CORPUS_H

#include "main.h"
#include "sphere.h"
#include "collision_type.h"

#define BLOCKING_WEIGHT ((Real)10000000)
#define GROUND_COLLISION_RADIUS ((Real)20)

class Collision;
class ArenaEntity;
class Unit;

class Corpus
{
public:
  Corpus(ArenaEntity*     a_owner = NULL,
         Ogre::SceneNode* a_scene_node = NULL);
  virtual ~Corpus();

  virtual bool update(const Real a_dt);

  // position
  Vector2 getXZ();
  size_t_pair getCellIndex();

  // move around
  void move(Vector3 a_move);
  // put it in the cell on the arena
  void updateCellIndex();

  // orientation
  void setOrientation(Quaternion a_orientation);
  Vector3 getDirection();

  // model
  void setSceneNode(Ogre::SceneNode* a_scene_node);
  void setOwner(ArenaEntity* a_owner);

  // damage and heat
  Real getBallisticDmg();
  Real getEnergyDmg();
  Real getHeatDmg();

  // collisions
  collision_type getCollisionType();
  Real getPenetration();
  Real getFriction();

  void updateBoundingSphere();
  void pruneCollisionSpheres(const Sphere&       a_sphere,
                             bitset<MAX_NUM_CS>& a_cs_bitset);

  // read collision spheres for the object TEMP
  void loadCollision(const string& aCollisionName);

  // resolve collisions
  bool validateCollision(Corpus* a_object);
  bool handleCollision(Collision* a_collision);
  void invalidateSpheres();

public:
  ArenaEntity* OwnerEntity = NULL;
  Ogre::SceneNode* SceneNode = NULL;

  // position and orientation
  Vector3 XYZ = Vector3::ZERO;
  Quaternion Orientation = Quaternion::IDENTITY;
  Vector3 Direction = Vector3::ZERO;
  Vector3 Velocity = Vector3::ZERO;
  Vector3 OldVelocity = Vector3::ZERO;
  // index of the cell in the arena the object is in
  size_t_pair CellIndex = { 0, 0 };

  // collision
  Sphere BoundingSphere;
  vector<Sphere> CollisionSpheres;
  // tags for spheres to differentiate parts
  vector<usint> CSAreas;
  // sphere positions relative to main scene node
  Vector3 RelBSPosition = Vector3::ZERO;
  vector<Vector3> RelCSPositions;

  collision_type CollisionType = collision_type_soft;
  Real Penetration = 0;
  Real SurfaceTemperature = 0;
  Real Friction = 0;
  Real Weight = 0;
  Real Hardness = 0;
  Real Conductivity = 0;

  Real BallisticDmg = 0;
  Real HeatDmg = 0;
  Real EnergyDmg = 0;

private:
  // speed and position
  Real corrected_velocity_scalar = 0;

  // need to recalculate from relative positions
  bool CSInvalid = true;
  bool BSInvalid = true;

public:
  bool OnArena = false;
  bool OutOfBounds = false;

  // debug
public:
  void displayCollision(bool a_toggle);
  void displayCollisionHit();
  void displayCollisionUpdate();

  vector<Ogre::Entity*> DebugCSEntities;
  vector<Ogre::SceneNode*> DebugCSNodes;
  Ogre::Entity* DebugBSEntity;
  Ogre::SceneNode* DebugBSNode;

  bool DisplayCollisionDebug;
};

inline Vector2 Corpus::getXZ()
{
  return Vector2(XYZ.x, XYZ.z);
}

inline size_t_pair Corpus::getCellIndex()
{
  return CellIndex;
}

inline void Corpus::move(Vector3 a_move)
{
  XYZ += a_move;
  invalidateSpheres();
}

inline void Corpus::setOrientation(Quaternion a_orientation)
{
  Orientation = a_orientation;
  Direction = Orientation * Vector3::UNIT_Z;
}

inline void Corpus::invalidateSpheres()
{
  BSInvalid = true;
  CSInvalid = true;
}

inline Vector3 Corpus::getDirection()
{
  return Direction;
}

inline Real Corpus::getBallisticDmg()
{
  return BallisticDmg;
}

inline Real Corpus::getEnergyDmg()
{
  return EnergyDmg;
}

inline Real Corpus::getHeatDmg()
{
  return HeatDmg;
}

// collisions
inline collision_type Corpus::getCollisionType()
{
  return CollisionType;
}

inline Real Corpus::getPenetration()
{
  return Penetration;
}

inline Real Corpus::getFriction()
{
  return Friction;
}

#endif // CORPUS_H
