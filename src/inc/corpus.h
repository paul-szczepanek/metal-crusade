// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CORPUS_H
#define CORPUS_H

#include "main.h"
#include "sphere.h"
#include "collision_type.h"

class Collision;
class ArenaEntity;
class Unit;

class Corpus
{
public:
  Corpus(ArenaEntity*     a_owner = NULL,
         Ogre::SceneNode* a_scene_node = NULL);
  virtual ~Corpus();

  virtual bool update(Real a_dt);

  // position
  Real getX();
  Real getY();
  Real getZ();
  Vector2 getXZ();
  Vector3 getXYZ();
  size_t_pair getCellIndex();

  // move around
  void setXYZ(Vector3 a_pos_xyz);
  void move(Vector3 a_move);
  Vector3 getVelocity();
  // put it in the cell on the arena
  void updateCellIndex();

  // orientation
  Quaternion getOrientation();
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

  Sphere getBoundingSphere();
  void pruneCollisionSpheres(const Sphere&       a_sphere,
                             bitset<MAX_NUM_CS>& a_cs_bitset);
  Real getCollisionSpheres(const Sphere&             a_sphere,
                           const bitset<MAX_NUM_CS>& a_valid_cs_bitset,
                           bitset<MAX_NUM_CS>&       a_cs_bitset);

  // read collision spheres for the object TEMP
  void loadCollisionSpheres(const string& aCollisionName);

  // resolve collisions
  bool validateCollision(Corpus* a_object);
  bool handleCollision(Collision* a_collision);
  bool revertMove(Vector3 a_move);
  void invalidateSpheres();

  /*mfd_view::diagram_type getDiagramType() {
     return mfd_view::object;
     }*/

public:
  ArenaEntity* OwnerEntity;
  Ogre::SceneNode* SceneNode;

  // position and orientation
  Vector3 XYZ;
  Quaternion Orientation;
  Vector3 Direction;
  Vector3 Velocity;
  Vector3 OldVelocity;

  // collision
  Sphere BoundingSphere;
  vector<Sphere> CollisionSpheres;
  // tags for spheres to differentiate parts
  vector<usint> CSAreas;
  // sphere positions relative to main scene node
  Vector3 RelBSPosition;
  vector<Vector3> RelCSPositions;

  collision_type CollisionType;
  Real Penetration;
  Real SurfaceTemperature;
  Real Friction;
  Real Weight;
  Real Hardness;
  Real Conductivity;

  Real BallisticDmg;
  Real HeatDmg;
  Real EnergyDmg;

private:
  // index of the cell in the arena the object is in
  size_t_pair CellIndex;

  // speed and position
  Vector3 Move;
  Real angular_momentum;
  Real corrected_velocity_scalar;

  // need to recalculate from relative positions
  bool CSInvalid;
  bool BSInvalid;

  bool out_of_bounds;

  // collision system
  bool registered;
  // if it's been collided with this frame
  bool collided;

  // debug
private:
  void displayCollision(bool a_toggle);
  void displayCollisionHit();
  void displayCollisionUpdate();

  vector<Ogre::Entity*> DebugCSEntities;
  vector<Ogre::SceneNode*> DebugCSNodes;
  Ogre::Entity* DebugBSEntity;
  Ogre::SceneNode* DebugBSNode;

  bool DisplayCollisionDebug;
};

// position
Real Corpus::getX()
{
  return XYZ.x;
}

Real Corpus::getY()
{
  return XYZ.y;
}

Real Corpus::getZ()
{
  return XYZ.z;
}

Vector2 Corpus::getXZ()
{
  return Vector2(XYZ.x, XYZ.z);
}

Vector3 Corpus::getXYZ()
{
  return XYZ;
}

size_t_pair Corpus::getCellIndex()
{
  return CellIndex;
}

// move around
void Corpus::setXYZ(Vector3 a_pos_xyz)
{
  XYZ = a_pos_xyz;
  invalidateSpheres();
}

void Corpus::move(Vector3 a_move)
{
  Move = a_move;
  XYZ += Move;
  Direction = Move;
  Direction.normalise();
  invalidateSpheres();
}

Vector3 Corpus::getVelocity()
{
  return Move;
}

// orientation
Quaternion Corpus::getOrientation()
{
  return Orientation;
}

void Corpus::setOrientation(Quaternion a_orientation)
{
  Orientation = a_orientation;
  Direction = Orientation * Vector3::UNIT_Z;
}

inline void Corpus::invalidateSpheres()
{
  BSInvalid = true;
  CSInvalid = true;
}

Vector3 Corpus::getDirection()
{
  return Direction;
}

Real Corpus::getBallisticDmg()
{
  return BallisticDmg;
}

Real Corpus::getEnergyDmg()
{
  return EnergyDmg;
}

Real Corpus::getHeatDmg()
{
  return HeatDmg;
}

// collisions
collision_type Corpus::getCollisionType()
{
  return CollisionType;
}

Real Corpus::getPenetration()
{
  return Penetration;
}

Real Corpus::getFriction()
{
  return Friction;
}

#endif // CORPUS_H
