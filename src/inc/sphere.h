// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef SPHERE_H
#define SPHERE_H

// sphere for use with collisions
// defined by a point and a Radius
// tells you what the distance is to another sphere
// and if it intersects with it

struct Sphere {
  Sphere()
    : Radius(1), Centre(Ogre::Vector3::ZERO) { };
  Sphere(Ogre::Real aRadius)
    : Radius(aRadius), Centre(Ogre::Vector3::ZERO) { };
  Sphere(Ogre::Real aX, Ogre::Real aY, Ogre::Real aZ, Ogre::Real aRadius)
    : Radius(aRadius), Centre(Ogre::Vector3(aX, aY, aZ)) { };
  Sphere(const Ogre::Vector3& aCentre, Ogre::Real aRadius)
    : Radius(aRadius), Centre(aCentre) { };

  bool intersects(const Sphere& sphere) const
  {
    return ((sphere.Centre - Centre).length() <= sphere.Radius + Radius);
  };

  bool contains(const Sphere& sphere) const
  {
    return ((sphere.Centre - Centre).length() + sphere.Radius <= Radius);
  };
  bool contains(const Ogre::Vector3& point) const
  {
    return ((point - Centre).length() <= Radius);
  };

  Ogre::Real distanceToSurface(const Sphere& sphere) const
  {
    return ((sphere.Centre - Centre).length() - sphere.Radius - Radius);
  };

  Ogre::Real Radius;
  Ogre::Vector3 Centre;
};

#endif // SPHERE_H


