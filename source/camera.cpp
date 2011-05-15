//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "camera.h"
#include "corpus.h"
#include "game.h"

const Ogre::Radian camera_fov(0.15); //near orthograhic
const Ogre::Real camera_lead = 40; //how far ahead of the unit to look

Camera::Camera() : position(0, 0, 0), look_at(0, 0, 0), look_at_offset(0, 0, 0)
{
    //create the Ogre camera
    ogre_camera = Game::scene->createCamera("camera");

    //camera clipping
    ogre_camera->setNearClipDistance(100); //the camera is 1600 above the ground - pretty safe clip
    ogre_camera->setFarClipDistance(10000);

    //camera fov - it ought to show a cicrle of 80m
    ogre_camera->setFOVy(camera_fov);

    //initial size and position
    resize(Game::render_window->getWidth(), Game::render_window->getHeight());
    ogre_camera->setPosition(position);
    ogre_camera->lookAt(look_at + look_at_offset);
}

/** @brief changes the angle of the camera when you resize the window
  * @todo: figure out what to do in portrait mode
  */
void Camera::resize(int a_width, int a_height)
{
    //calculate aspect ratio
    Ogre::Real aspect_ratio = Ogre::Real(a_width)/Ogre::Real(a_height);
    Ogre::Real aspect_ratio_inverse = Ogre::Real(1) / aspect_ratio;

    //aply it to the ogre camera
    ogre_camera->setAspectRatio(aspect_ratio);

    //angle between camera ray and ground normal
    Ogre::Real camera_angle;

    //adapt to new aspect ratio - camera ought to always show 160x160 meters sqaure
    if (aspect_ratio > 1) { //is it landscape
        camera_angle = Ogre::Real(acos(aspect_ratio_inverse));

        //make sure it's within reasonable bounds
        if (camera_angle < 0.5) {
            camera_angle = 0.5;
        } else if (camera_angle > 1.2) {
            camera_angle = 1.2;
        }

    } else {
        //if portrait ratio cap the angle to 0.5 to stop acos tripping up
        camera_angle = 0.5;
    }

    //position the camera according to the angle and distance
    //temp sideways camera
    //camera_offset = Ogre::Vector3(-sin(camera_angle) * camera_distance *5* aspect_ratio_inverse,
    //                              cos(camera_angle) * camera_distance *5* aspect_ratio_inverse,
    //                              0);

    camera_offset = Ogre::Vector3(0, cos(camera_angle) * camera_distance *5* aspect_ratio_inverse,
                                  sin(camera_angle) * camera_distance *15* aspect_ratio_inverse);

    //height of camera target depends on angle
    look_at_offset = Ogre::Vector3(0, -5 * (camera_angle / (0.5 * pi)), 0);
    //-5 is a magic number chosen to show the action best assuming the units are roughly 10m high

    //immidiately snap to new camera position rather than lag
    position = look_at + camera_offset;
}

/** @brief add an object to follow with the camera
  */
void Camera::follow(Corpus* a_corpus)
{
    follow_list.push_back(a_corpus);

    //if first unit to follow snap the camera to it
    if (follow_list.size() == 1) {
        update(2.0);
    }
}

/** @brief move camera to where the unit is looking
  * @todo: zoom to extents of all followed units so that none is outside the viewport
  */
void Camera::update(Ogre::Real a_dt)
{
    //midpoints with all followed units in a cheap and nasty way TODO: zooming to extents
    for (usint i = 0, for_size = follow_list.size(); i < for_size; ++i) {
        //see where the unit is pointing and point camera ahead of it
        Ogre::Vector3 new_look_at = (camera_lead * follow_list.at(i)->getDirection())
                                    + follow_list.at(i)->getPosition();

        //midpoint with old camera pos or previous unit
        look_at += (new_look_at - look_at) * a_dt * 0.5;

        //add offset to camera position to maintain angle
        Ogre::Vector3 new_position = look_at + camera_offset;

        //make the position lag behind focus point
        position += (new_position - position) * a_dt * 0.25;
    }

    //commit new positions
    ogre_camera->setPosition(position);
    ogre_camera->lookAt(look_at + look_at_offset);
}
