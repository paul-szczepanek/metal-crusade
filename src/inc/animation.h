// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef ANIMATION_H
#define ANIMATION_H

#include "main.h"

const usint num_of_movement_modes = 3;
const usint num_of_animations = 4;
const usint num_of_animated_body_parts = 7;
const string animation_names[num_of_animations] = { "stand", "walk", "run", "crouch" };

namespace animation
{
enum crusader { // for indexing arrays
  stand,
  walk,
  run,
  crouch
};

};

class Animation
{
public:
  Animation(Ogre::SceneNode* a_scene_node);
  ~Animation()
  {
  }

  void update(Real a_dt);

  // setting the animation speed - type is set automatically
  void walk(Real a_speed);
  void crouch();
  void turn(Radian a_turning_speed);

private:
  void extractAnimations(Ogre::SceneNode* a_scene_node);
  bool switchAnimations(Real a_time_to_add);

  // current status
  bool stopped;
  Real cycle_length;
  Real rate;
  animation::crusader current_animation;
  animation::crusader target_animation;
  Real switching_progress;

  // animation data
  vector<Ogre::AnimationState*> animations[num_of_animations];
  Real movement_speeds[num_of_movement_modes];
  Real movement_speed_limits[num_of_movement_modes];
  Real switch_points[2];
  Real stand_points[2];
  // vector<Real> footsteps; // when the mech hits the ground TODO
};

#endif // ANIMATION_H
