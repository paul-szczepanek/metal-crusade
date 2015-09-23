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
  Corpus(Ogre::Vector3 aPosXyz, Ogre::Quaternion aOrientation,
         Ogre::SceneNode* aSceneNode, const string& aCollisionName, ArenaEntity* aOwner);
  virtual ~Corpus();

  // main loop
  virtual int update(Ogre::Real a_dt);

  // position
  Ogre::Real getX() { return PosXyz.x; };
  Ogre::Real getY() { return PosXyz.y; };
  Ogre::Real getZ() { return PosXyz.z; };
  Ogre::Vector2 getXZ() { return Ogre::Vector2(PosXyz.x, PosXyz.z); };
  Ogre::Vector3 getPosition() { return PosXyz; };
  uint_pair getCellIndex() { return CellIndex; };

  // move around
  void setXYZ(Ogre::Vector3 a_pos_xyz) { pos_xyz = a_pos_xyz; };
  void setVelocity(Ogre::Vector3 a_velocity) { velocity = a_velocity; };
  Ogre::Vector3 getVelocity() { return velocity; };

  // orientation
  Ogre::Quaternion getOrientation() { return Orientation; };
  virtual Ogre::Quaternion getLookingOrientation() { return Orientation; };
  virtual Ogre::Vector3 getDirection();

  // corpus ignores any request to react to collisions
  virtual int handleCollision(Collision* a_collision);
  virtual Ogre::Vector3 getVelocity() { return Ogre::Vector3::ZERO; };
  virtual Ogre::Real getWeight() { return 10000; };
  virtual bool revertMove(Ogre::Vector3 a_move) { return false; };

  // hud operation
  virtual Ogre::Real getSpeed() { return corrected_velocity_scalar; };
  virtual Ogre::Real getEngineTemperature()  { return 0; };
  virtual Ogre::Real getCoreTemperature()  { return 0; };
  virtual Ogre::Real getPressure()  { return core_integrity; };

  // partitionening into cells on the arena made
  virtual void updateCellIndex();

  // collision handling
  void collideWith(Corpus* a_thing) { collided_with.push_back(a_thing); }
  bool hasCollidedWith(Corpus* a_thing);

  // damage and heat
  virtual Ogre::Real getSurfaceTemperature() { return SurfaceTemperature; };
  void setSurfaceTemperature(Ogre::Real aSurfaceTemperature) ;
  virtual Ogre::Real getBallisticDmg() { return 0; };
  virtual Ogre::Real getEnergyDmg() { return 0; };
  virtual Ogre::Real getHeatDmg() { return 0; };

  // collisions
  collision_type getCollisionType() { return CollisionType; };
  Ogre::Real getPenetration() { return Penetration; };
  Ogre::Real getFriction() { return Friction; };
  virtual bool validateCollision(Corpus* a_colliding_object) { return true; } ;
  virtual Sphere getBoundingSphere();
  virtual bitset<MAX_NUM_ES> getExclusionSpheres(Sphere& sphere);
  virtual bitset<MAX_NUM_CS> getCollisionSpheres(Sphere& sphere);
  virtual bitset<MAX_NUM_CS> getCollisionSpheres(Sphere& sphere, bitset<MAX_NUM_ES> es_bitset);
  bool getCollisionSpheres(const string& filename);

  // put it in the cell on the arena
  void updateCellIndex();

  // read collision spheres for the object TEMP
  virtual void loadCollisionSpheres(const string& aCollisionName);

  // main ogre scene node for the object with synced position
  Ogre::SceneNode* SceneNode;

  vector<Sphere> CollisionSpheres;

  // resolve collisions
  virtual int handleCollision(Collision* a_collision);
  bool revertMove(Ogre::Vector3 a_move);

  // weight
  Ogre::Real getWeight() { return total_weight; };

  // targetting

  // called by other object to try and aqcuire this as a target
  bool acquireAsTarget(Corpus* a_targeted_by);
  // called by other objects which hold this as a target to let it know that they no longer do
  void releaseAsTarget(Corpus* a_targeted_by);
  // called by targeted object that requires this to relinquish its current target
  bool loseTarget(Corpus* a_targeted_by, bool a_forced = false);

  // radar
  virtual bool isDetectable() { return true; };

  // return the target of this
  Corpus* getTarget() { return target; };

  // damage reporting
  virtual Ogre::Real getDamage(usint a_diagram_element) { return core_integrity; };
  virtual mfd_view::diagram_type getDiagramType() { return mfd_view::object; };

private:
  void displayCollision(bool a_toggle);
  void displayCollisionHit();
  void displayCollisionUpdate();

private:
  // main loop
  virtual int updateController();

  // clear targets when this object gets destroyed
  void clearFromTargets();

  // targeted objects
  Corpus* target;

  // targeted by objects
  vector<Corpus*> target_holders;

  ArenaEntity* Owner;

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
  Ogre::Vector3 RelBSPosition;
  vector<Ogre::Vector3> RelESPositions;
  vector<Ogre::Vector3> RelCSPositions;

  // time elapsed since last frame
  Ogre::Real Dt;

  // position and orientation
  Ogre::Vector3 PosXyz;
  Ogre::Quaternion Orientation;
  Ogre::Vector3 Direction;

  // collision
  collision_type CollisionType;
  Ogre::Real Penetration;
  Ogre::Real SurfaceTemperature;
  Ogre::Real Friction;
  Ogre::Real Conductivity;

  // index of the cell in the arena the object is in
  uint_pair CellIndex;

  // speed and position
  Ogre::Vector3 velocity;
  Ogre::Vector3 move;
  Ogre::Real angular_momentum;
  Ogre::Real corrected_velocity_scalar;
  bool out_of_bounds;

  // weight
  Ogre::Real total_weight;

  // collision system
  bool registered;
  // if it's been collided with this frame
  bool collided;
  // list of objects collided with this frame
  vector<Corpus*> collided_with;

  // hit points
  Ogre::Real core_integrity;

  // debug
  vector<Ogre::Entity*> DebugCSEntities;
  vector<Ogre::SceneNode*> DebugCSNodes;
  Ogre::Entity* DebugBSEntity;
  Ogre::SceneNode* DebugBSNode;

private:
  bool DisplayCollisionDebug;
};

inline void Corpus::setSurfaceTemperature(Ogre::Real aSurfaceTemperature)
{
  SurfaceTemperature = aSurfaceTemperature;
};

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
