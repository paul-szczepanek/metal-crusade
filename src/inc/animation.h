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
  Animation();
  ~Animation();

  void update(const Real a_dt);

  void extractAnimations(Ogre::SceneNode* a_scene_node);

  // setting the animation speed - type is set automatically
  void walk(Real a_speed);
  void crouch();
  void turn(Radian a_turning_speed);

private:
  bool switchAnimations(Real a_time_to_add);

  // current status
  bool Stopped;
  Real CycleLength;
  Real Rate;
  animation::crusader CurrentAnim;
  animation::crusader TargetAnim;
  Real SwitchingProgress;

  // animation data
  vector<Ogre::AnimationState*> Animations[num_of_animations];
  Real MovementSpeeds[num_of_movement_modes];
  Real MovementSpeedLimits[num_of_movement_modes];
  Real SwitchPoints[2];
  Real StandPoints[2];
  // vector<Real> Footsteps; // when the mech hits the ground TODO
};

#endif // ANIMATION_H
