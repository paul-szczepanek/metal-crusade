// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game_camera.h"
#include "corpus.h"
#include "game.h"

static const Ogre::Radian CAMERA_FOV(0.15); // near orthograhic
static const Ogre::Real CAMERA_LEAD = 40; // how far ahead of the unit to look

GameCamera::GameCamera()
{
  // create the Ogre camera
  OgreCamera = Game::OgreScene->createCamera("camera");

  // camera clipping
  OgreCamera->setNearClipDistance(100); // the camera is 1600 above the ground - pretty safe clip
  OgreCamera->setFarClipDistance(10000);

  // camera fov - it ought to show a cicrle of 80m
  OgreCamera->setFOVy(CAMERA_FOV);

  // initial size and position
  resize(Game::OgreWindow->getWidth(), Game::OgreWindow->getHeight());
  OgreCamera->setPosition(Position);
  OgreCamera->lookAt(LookAt + LookAtOffset);
}

/** @brief changes the angle of the camera when you resize the window
  * @todo: figure out what to do in portrait mode
  */
void GameCamera::resize(int aWidth, int aHeight)
{
  // calculate aspect ratio
  Ogre::Real aspect_ratio = Ogre::Real(aWidth) / Ogre::Real(aHeight);
  Ogre::Real aspect_ratio_inverse = Ogre::Real(1) / aspect_ratio;

  // apply it to the ogre camera
  OgreCamera->setAspectRatio(aspect_ratio);

  // angle between camera ray and ground normal
  Ogre::Real camera_angle;

  // adapt to new aspect ratio - camera ought to always show 160x160 meters sqaure
  if (aspect_ratio > 1) { // is it landscape
    camera_angle = Ogre::Real(acos(aspect_ratio_inverse));

    // make sure it's within reasonable bounds
    if (camera_angle < 0.5) {
      camera_angle = 0.5;
    } else if (camera_angle > 1.2) {
      camera_angle = 1.2;
    }

  } else {
    // if portrait ratio cap the angle to 0.5 to stop acos tripping up
    camera_angle = 0.5;
  }

  // position the camera according to the angle and distance
  // temp sideways camera
  // CameraOffset = Ogre::Vector3(-sin(camera_angle) * camera_distance *5* aspect_ratio_inverse,
  //                              cos(camera_angle) * camera_distance *5* aspect_ratio_inverse,
  //                              0);

  CameraOffset = Ogre::Vector3(0, cos(camera_angle) * CAMERA_DIST * aspect_ratio_inverse,
                               sin(camera_angle) * CAMERA_DIST * aspect_ratio_inverse);

  // height of camera target depends on angle
  LookAtOffset = Ogre::Vector3(0, -5 * (camera_angle / (0.5 * pi)), 0);
  // -5 is a magic number chosen to show the action best assuming the units are roughly 10m high

  // immidiately snap to new camera position rather than lag
  Position = LookAt + CameraOffset;
}

/** @brief add an object to follow with the camera
  */
void GameCamera::follow(Corpus* aCorpus)
{
  FollowList.push_back(aCorpus);

  // if first unit to follow snap the camera to it
  if (FollowList.size() == 1) {
    update(2.0);
  }
}

/** @brief move camera to where the unit is looking
  * @todo: zoom to extents of all followed units so that none is outside the viewport
  */
void GameCamera::update(Ogre::Real aDt)
{
  // midpoints with all followed units in a cheap and nasty way TODO: zooming to extents
  for (usint i = 0, for_size = FollowList.size(); i < for_size; ++i) {
    // see where the unit is pointing and point camera ahead of it
    Ogre::Vector3 new_LookAt = (CAMERA_LEAD * FollowList.at(i)->getDirection())
                               + FollowList.at(i)->getPosition();

    // midpoint with old camera pos or previous unit
    LookAt += (new_LookAt - LookAt) * aDt * 0.5;

    // add offset to camera position to maintain angle
    Ogre::Vector3 new_position = LookAt + CameraOffset;

    // make the position lag behind focus point
    Position += (new_position - Position) * aDt * 0.25;
  }

  // commit new positions
  OgreCamera->setPosition(Position);
  OgreCamera->lookAt(LookAt + LookAtOffset);
}
