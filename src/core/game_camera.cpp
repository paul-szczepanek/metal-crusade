// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game_camera.h"
#include "unit.h"
#include "game.h"

static const Radian CAMERA_FOV(0.15); // near orthograhic
static const Real CAMERA_LEAD = 40; // how far ahead of the unit to look

GameCamera::~GameCamera()
{
  delete OgreCamera;
}

GameCamera::GameCamera()
{
  // create the Ogre camera
  OgreCamera = Game::Scene->createCamera("camera");

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
void GameCamera::resize(int aWidth,
                        int aHeight)
{
  // calculate aspect ratio
  Real aspect_ratio = Real(aWidth) / Real(aHeight);
  Real aspect_ratio_inverse = Real(1) / aspect_ratio;

  // apply it to the ogre camera
  OgreCamera->setAspectRatio(aspect_ratio);

  // angle between camera ray and ground normal
  Real camera_angle;

  // adapt to new aspect ratio - camera ought to always show 160x160 meters sqaure
  if (aspect_ratio > 1) { // is it landscape
    camera_angle = Real(acos(aspect_ratio_inverse));

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
  // CameraOffset = Vector3(-sin(camera_angle) * CameraDistance *5* aspect_ratio_inverse,
  //                              cos(camera_angle) * CameraDistance *5* aspect_ratio_inverse,
  //                              0);

  CameraOffset = Vector3(0, cos(camera_angle) * CameraDistance * aspect_ratio_inverse,
                         sin(camera_angle) * CameraDistance * aspect_ratio_inverse);

  // height of camera target depends on angle
  LookAtOffset = Vector3(0, -5 * (camera_angle / (0.5 * pi)), 0);
  // -5 is a magic number chosen to show the action best assuming the units are roughly 10m high

  // immediately snap to new camera position rather than lag
  Position = LookAt + CameraOffset;
}

/** @brief add an object to follow with the camera
 */
void GameCamera::follow(Unit* a_entity)
{
  FollowList.push_back(a_entity);

  // if first unit to follow snap the camera to it
  if (FollowList.size() == 1) {
    update(2.0);
  }
}

/** @brief move camera to where the unit is looking
 * @todo: zoom to extents of all followed units so that none is outside the viewport
 */
void GameCamera::update(Real aDt)
{
  // midpoints with all followed units in a cheap and nasty way TODO: zooming to extents
  for (size_t i = 0, for_size = FollowList.size(); i < for_size; ++i) {
    // see where the unit is pointing and point camera ahead of it
    Vector3 new_LookAt = (CAMERA_LEAD * FollowList.at(i)->getDirection())
                         + FollowList.at(i)->getXYZ();

    // midpoint with old camera pos or previous unit
    LookAt += (new_LookAt - LookAt) * aDt * 0.5;

    // add offset to camera position to maintain angle
    Vector3 new_position = LookAt + CameraOffset;

    // make the position lag behind focus point
    Position += (new_position - Position) * aDt * 0.25;
  }

  // commit new positions
  OgreCamera->setPosition(Position);
  OgreCamera->lookAt(LookAt + LookAtOffset);
}
