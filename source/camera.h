//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef CAMERA_H
#define CAMERA_H

#include "main.h"

class Corpus;

class Camera
{
public:
    Camera();
    ~Camera() { delete(ogre_camera); };

    //main loop
    void update(Ogre::Real a_dt);

    //set target(s) to follow
    void follow(Corpus* a_corpus);

    //called on screen resize
    void resize(int a_width, int a_height);

    //expose the ogre camera to appease the Ogre
    Ogre::Camera* getOgreCamera() { return ogre_camera; };
    Ogre::Quaternion getOrientation() { return ogre_camera->getOrientation(); };

private:
    Ogre::Camera* ogre_camera;

    //objects kept within the camera frame
    vector<Corpus*> follow_list;

    //camera positio and direction
    Ogre::Vector3 camera_offset;
    Ogre::Vector3 position;
    Ogre::Vector3 look_at;

    //this is a magic offset to compensate for the fact we're actually interested
    //in what is above ground not the ground itself
    Ogre::Vector3 look_at_offset;
};

const Ogre::Real camera_distance(14000); //to show a circle of 80m

#endif // CAMERA_H
