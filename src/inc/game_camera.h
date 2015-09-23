// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CAMERA_H
#define CAMERA_H

#include "main.h"

class Corpus;

class GameCamera
{
public:
  GameCamera();
  ~GameCamera() { delete OgreCamera; };

  // main loop
  void update(Ogre::Real aDt);

  // set target(s) to follow
  void follow(Corpus* aCorpus);

  // called on screen resize
  void resize(int aWidth, int aHeight);

  // expose the ogre camera to appease the Ogre
  Ogre::Camera* getOgreCamera() { return OgreCamera; };
  Ogre::Quaternion getOrientation() { return OgreCamera->getOrientation(); };

private:
  Ogre::Camera* OgreCamera;

  // objects kept within the camera frame
  vector<Corpus*> FollowList;

  // camera positio and direction
  Ogre::Vector3 CameraOffset;
  Ogre::Vector3 Position;
  Ogre::Vector3 LookAt;

  // this is a magic offset to compensate for the fact we're actually interested
  // in what is above ground not the ground itself
  Ogre::Vector3 LookAtOffset;
};

const Ogre::Real CAMERA_DIST(1400); // to show a circle of 80m

#endif // CAMERA_H
