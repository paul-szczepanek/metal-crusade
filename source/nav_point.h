#ifndef NAVPOINT_H
#define NAVPOINT_H

#include "imaginarius.h"


class NavPoint : public Imaginarius
{
public:
    NavPoint(Ogre::Vector3 a_pos_xyz, string a_name = "navpoint x");
    virtual ~NavPoint();

private:
    string name;
};

#endif // NAVPOINT_H
