//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef SPHERE_H
#define SPHERE_H

//sphere for use with collisions
//defined by a point and a radius
//tells you wha the distance is to another sphere
//and if it intersects with it

class Sphere
{
public:
    Sphere()
        : radius(1), centre(Ogre::Vector3::ZERO) { };
    Sphere(Ogre::Real a_radius)
        : radius(a_radius), centre(Ogre::Vector3::ZERO) { };
    Sphere(Ogre::Real a_x, Ogre::Real a_y, Ogre::Real a_z, Ogre::Real a_radius)
        : radius(a_radius), centre(Ogre::Vector3(a_x, a_y, a_z)) { };
    Sphere(const Ogre::Vector3& a_centre, Ogre::Real a_radius)
        : radius(a_radius), centre(a_centre) { };

    bool intersects(const Sphere& sphere) const {
        return ((sphere.centre - centre).length() <= sphere.radius + radius);
    };
    bool contains(const Sphere& sphere) const {
        return ((sphere.centre - centre).length() + sphere.radius <= radius);
    };
    bool contains(const Ogre::Vector3& point) const {
        return ((point - centre).length() <= radius);
    };
    Ogre::Real distanceToSurface(const Sphere& sphere) const {
        return ((sphere.centre - centre).length() - sphere.radius - radius);
    };

    Ogre::Real radius;
    Ogre::Vector3 centre;
};

#endif // SPHERE_H


