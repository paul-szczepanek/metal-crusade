// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "animation.h"
#include "game.h"

// speed at which you consider to be not moving
const Real crusader_stopping_speed = 0.001;
const Real switching_time = 1;

Animation::~Animation()
{
}

Animation::Animation()
  : Stopped(true), CycleLength(2), Rate(0),
  CurrentAnim(animation::stand), TargetAnim(animation::stand), SwitchingProgress(0)
{
  for (size_t i = 0; i < num_of_movement_modes; ++i) {
    Animations[0].reserve(num_of_animated_body_parts);
  }

  // TODO: read this from an animation definition file
  MovementSpeeds[0] = 2;
  MovementSpeeds[1] = 0.3;
  MovementSpeeds[2] = 0.13;
  // at what speed to start each animation
  MovementSpeedLimits[0] = 0;
  MovementSpeedLimits[1] = 0.1;
  MovementSpeedLimits[2] = 14;
  // points when you can switch animations
  SwitchPoints[0] = CycleLength * 0.5;
  SwitchPoints[1] = CycleLength * 1.5;
  // points where the animations stands on the ground
  StandPoints[0] = 0;
  StandPoints[1] = CycleLength;
}

/** @brief get all the crusaders animations and stick em in the vector for easier reference
 */
void Animation::extractAnimations(Ogre::SceneNode* a_scene_node)
{
  // get animation states from all entities
  for (size_t i = 0; i < num_of_animations; ++i) {
    Ogre::SceneNode::ObjectIterator it = a_scene_node->getAttachedObjectIterator();
    while (it.hasMoreElements()) {
      // get the mesh attached to the scene node
      Ogre::Entity* anim_entity = static_cast<Ogre::Entity*>(it.getNext());

      // safeguard for ommited anims
      if (NULL != anim_entity->getAllAnimationStates()) {
        // stick the animation state in the array later indexed by animation type enum
        Animations[i].push_back(anim_entity->getAnimationState(animation_names[i]));
        // reset the animation
        Animations[i].back()->setTimePosition(0);
        if (i == CurrentAnim) {
          Animations[i].back()->setEnabled(true);
        }
      }
    }
  }

  // recurse into child scene nodes
  Ogre::SceneNode::ChildNodeIterator it2 = a_scene_node->getChildIterator();
  while (it2.hasMoreElements()) {
    extractAnimations(static_cast<Ogre::SceneNode*>(it2.getNext()));
  }
}

/** @brief move forward or backward
 */
void Animation::walk(Real a_speed)
{
  // get speed forward or backward
  Real speed_abs = abs(a_speed);

  if (speed_abs < crusader_stopping_speed) {
    // use standing animation for stopping
    if (SwitchingProgress == 0) {
      TargetAnim = animation::stand;
    }
    Rate = 0;

  } else {
    // if it's moving find the fastest possible animation
    Stopped = false; // get moving
    if (SwitchingProgress == 0) {
      for (size_t i = 0; i < num_of_movement_modes; ++i) {
        if (speed_abs > MovementSpeedLimits[i]) { // find the fastest speed limit
          TargetAnim = static_cast<animation::crusader>(i); // set the target anim
        }
      }
    }

    // the rate depends on the actual speed
    Rate = a_speed * MovementSpeeds[CurrentAnim];
  }
}

/** @brief crouch, needs to stop first
 */
void Animation::crouch()
{
  TargetAnim = animation::crouch;
}

/** @brief turn around, forces anim if standing in place
 */
void Animation::turn(Radian a_turning_speed)
{
  if (CurrentAnim == animation::stand) {
    // equivalent rate of animation for turning
    Real turning_rate = -10 * a_turning_speed.valueRadians();

    // use turning rate or moving rate for anim - whichever is faster
    if (Rate > 0) {
      // when moving forward
      if (turning_rate > Rate || turning_rate < -Rate) {
        Rate = turning_rate;
        Stopped = false;
      }
    } else {
      // when moving backward
      if (-turning_rate < Rate || -turning_rate > -Rate) {
        Rate = -turning_rate;
        Stopped = false;
      }
    }
  }
}

/** @brief blends animations between different walking modes: stand, walk, run
 * @todo: deal with crouch and collisions
 * @todo: disable anim when not on screen
 * @return whether the current_animation == target_animation
 */
bool Animation::switchAnimations(Real a_time_to_add)
{
  if (CurrentAnim != TargetAnim || SwitchingProgress > 0) {
    // get keyframe
    Real animation_time = Animations[CurrentAnim][0]->getTimePosition();

    // check if the time passed contains the switch point
    bool crossed_switch1 = (animation_time >= SwitchPoints[0] &&
                            animation_time - a_time_to_add <= SwitchPoints[0]);
    bool crossed_switch2 = (animation_time >= SwitchPoints[1] &&
                            animation_time - a_time_to_add <= SwitchPoints[1]);

    // continue blending
    SwitchingProgress += abs(a_time_to_add);

    // if either switch passed or time limit reached change animation
    if (crossed_switch1 || crossed_switch2 || SwitchingProgress >= switching_time) {
      // disable old anims
      for (size_t i = 0, for_size = Animations[CurrentAnim].size(); i < for_size; ++i) {
        Animations[CurrentAnim][i]->setWeight(0);
        Animations[CurrentAnim][i]->setEnabled(false);
      }

      // switch anims
      CurrentAnim = TargetAnim;

      // animation switched so no need to continue blending
      SwitchingProgress = 0;

      // turn the new anims on
      for (size_t i = 0, for_size = Animations[CurrentAnim].size(); i < for_size; ++i) {
        Animations[CurrentAnim][i]->setEnabled(true);
        Animations[CurrentAnim][i]->setWeight(1);

        // pick up the anim where the last one left off
        Animations[CurrentAnim][i]->setTimePosition(animation_time);
      }
    } else {
      // apply wieghts to blend animations smoothly
      Real anim_weight = SwitchingProgress / switching_time;

      for (size_t i = 0, for_size = Animations[CurrentAnim].size(); i < for_size; ++i) {
        Animations[TargetAnim][i]->setEnabled(true);
        Animations[CurrentAnim][i]->setWeight(1 - anim_weight);
        Animations[TargetAnim][i]->setWeight(anim_weight);
        Animations[TargetAnim][i]->setTimePosition(animation_time);
      }

      return false; // still trying to switch animations
    }
  }
  return true; // switched animations or no need to
}

/** @brief progresses the animation
 */
void Animation::update(const Real a_dt)
{
  if (!Stopped) {
    if (Rate == 0) { // if trying to stop ignore the speed and keep animating
      // get keyframe
      Real animation_time = Animations[CurrentAnim][0]->getTimePosition();

      // did it cross the standing point
      bool crossed_stand1 = (animation_time >= StandPoints[0] &&
                             animation_time - a_dt <= StandPoints[0]);
      bool crossed_stand2 = (animation_time >= StandPoints[1] &&
                             animation_time - a_dt <= StandPoints[1]);

      // check if it's already in stand anim and crossed the standing position
      if (switchAnimations(a_dt) && (TargetAnim == animation::stand)
          && (crossed_stand1 || crossed_stand2)) {
        // managed to stop
        Stopped = true;

        // reset the stand anim
        for (size_t i = 0, size = Animations[CurrentAnim].size(); i < size; ++i) {
          Animations[CurrentAnim][i]->setTimePosition(0);
        }
      } else {
        // keep animating until you hit these points and are in anim stand
        for (size_t i = 0, size = Animations[CurrentAnim].size(); i < size; ++i) {
          Animations[CurrentAnim][i]->addTime(a_dt);
        }
      }
    } else {
      // if moving translate time passed to animation time
      Real time_to_add = a_dt * Rate;

      // add the time to animations
      for (size_t i = 0, for_size = Animations[CurrentAnim].size(); i < for_size; ++i) {
        Animations[CurrentAnim][i]->addTime(time_to_add);
      }

      switchAnimations(time_to_add);
    }
  }
}
