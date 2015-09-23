// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef SPHERE_H
#define SPHERE_H

// sphere for use with collisions
// defined by a point and a Radius
// tells you what the distance is to another sphere
// and if it intersects with it

struct Sphere {
  Sphere()
    : Radius(1), Centre(Vector3::ZERO) {
  }

  Sphere(Real aRadius)
    : Radius(aRadius), Centre(Vector3::ZERO) {
  }

  Sphere(Real aX,
         Real aY,
         Real aZ,
         Real aRadius)
    : Radius(aRadius), Centre(Vector3(aX, aY, aZ)) {
  }

  Sphere(const Vector3& aCentre,
         Real           aRadius)
    : Radius(aRadius), Centre(aCentre) {
  }

  bool intersects(const Sphere& sphere) const
  {
    return ((sphere.Centre - Centre).length() <= sphere.Radius + Radius);
  }

  bool contains(const Sphere& sphere) const
  {
    return ((sphere.Centre - Centre).length() + sphere.Radius <= Radius);
  }

  bool contains(const Vector3& point) const
  {
    return ((point - Centre).length() <= Radius);
  }

  Real distanceToSurface(const Sphere& sphere) const
  {
    return ((sphere.Centre - Centre).length() - sphere.Radius - Radius);
  }

  Real Radius;
  Vector3 Centre;
};

#endif // SPHERE_H
