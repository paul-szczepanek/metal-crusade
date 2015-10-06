// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CAMERA_H
#define CAMERA_H

#include "main.h"

#define DEFAULT_CAMERA_DISTANCE (1400) // to show a circle of 80m

class Unit;

class GameCamera
{
public:
  GameCamera();
  ~GameCamera();

  void update(const Real a_dt);

  // set target(s) for the camera to follow
  void follow(Unit* a_entity);

  // called on screen resize
  void resize(int a_width,
              int a_height);

  Ogre::Camera* getOgreCamera();
  Quaternion getOrientation();

public:
  Real CameraDistance = DEFAULT_CAMERA_DISTANCE;

private:
  Ogre::Camera* OgreCamera;

  // objects kept within the camera frame
  vector<Unit*> FollowList;

  // camera positio and direction
  Vector3 CameraOffset;
  Vector3 Position;
  Vector3 LookAt;
  // this is a magic offset to compensate for the fact we're actually interested
  // in what is above ground not the ground itself
  Vector3 LookAtOffset;
};

inline Ogre::Camera* GameCamera::getOgreCamera()
{
  return OgreCamera;
}

inline Quaternion GameCamera::getOrientation()
{
  return OgreCamera->getOrientation();
}

#endif // CAMERA_H
