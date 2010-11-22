//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef IMAGINARIUS_H
#define IMAGINARIUS_H

#include "main.h"
#include "sphere.h"

class Imaginarius
{
public:
    Imaginarius(Ogre::Vector3 a_pos_xyz = Ogre::Vector3::ZERO);
    virtual ~Imaginarius() { };

    //position
    Ogre::Vector3 getPosition() { return pos_xyz; };

protected:
    string getUniqueID();

    //position and orientation
    Ogre::Vector3 pos_xyz;

    static ulint unique_id;
};

#endif // IMAGINARIUS_H
