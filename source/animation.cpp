//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "animation.h"
#include "game.h"

//speed at which you consider to be not moving
const Ogre::Real crusader_stopping_speed = 0.001;
const Ogre::Real switching_time = 1;

Animation::Animation(Ogre::SceneNode* a_scene_node)
    : stopped(true), cycle_length(2), rate(0),
    current_animation(animation::stand), target_animation(animation::stand), switching_progress(0)
{
    for (usint i = 0; i < num_of_movement_modes; ++i) {
        animations[0].reserve(num_of_animated_body_parts);
    }

    //TODO: read this from an animation definition file
    movement_speeds[0] = 2;
    movement_speeds[1] = 0.3;
    movement_speeds[2] = 0.13;
    //at what speed to start each animation
    movement_speed_limits[0] = 0;
    movement_speed_limits[1] = 0.1;
    movement_speed_limits[2] = 14;
    //points when you can switch animations
    switch_points[0] = cycle_length * 0.5;
    switch_points[1] = cycle_length * 1.5;
    //points where the animations stands on the ground
    stand_points[0] = 0;
    stand_points[1] = cycle_length;

    //get animation states from all the entities in the crusader
    extractAnimations(a_scene_node);

    //turn the initial anim on
    for (usint i = 0, for_size = animations[current_animation].size(); i < for_size; ++i) {
        animations[current_animation][i]->setEnabled(true);
    }
}

/** @brief get all the crusaders animations and stick em in the vector for easier reference
  */
void Animation::extractAnimations(Ogre::SceneNode* a_scene_node)
{
    //get animation states from all entities
    for (usint i = 0; i < num_of_animations; ++i) {
        Ogre::SceneNode::ObjectIterator it = a_scene_node->getAttachedObjectIterator();
        while (it.hasMoreElements()) {
            //get the mesh attached to the scene node
            Ogre::Entity* anim_entity = static_cast<Ogre::Entity*>(it.getNext());

            //safeguard for ommited anims
            if (NULL != anim_entity->getAllAnimationStates()) {
                //stick the animation state in the array later indexed by animation type enum
                animations[i].push_back(anim_entity->getAnimationState(animation_names[i]));
                //reset the animation
                animations[i].back()->setTimePosition(0);
            }
        }
    }

    //recurse into child scene nodes
    Ogre::SceneNode::ChildNodeIterator it2 = a_scene_node->getChildIterator();
    while (it2.hasMoreElements()) {
        extractAnimations(static_cast<Ogre::SceneNode*>(it2.getNext()));
    }
}

/** @brief move forward or backward
  */
void Animation::walk(Ogre::Real a_speed)
{
    //get speed forward or backward
    Ogre::Real speed_abs = abs(a_speed);

    if (speed_abs < crusader_stopping_speed) {
        //use standing animation for stopping
        if (switching_progress == 0) {
            target_animation = animation::stand;
        }
        rate = 0;

    } else {
        //if it's moving find the fastest possible animation
        stopped = false; //get moving
        if (switching_progress == 0) {
            for (usint i = 0; i < num_of_movement_modes; ++i) {
                if (speed_abs > movement_speed_limits[i]) { //find the fastest speed limit
                    target_animation = static_cast<animation::crusader>(i); //set the target anim
                }
            }
        }

        //the rate depends on the actual speed
        rate = a_speed * movement_speeds[current_animation];
    }
}

/** @brief crouch, needs to stop first
  */
void Animation::crouch()
{
    target_animation = animation::crouch;
}

/** @brief turn around, forces anim if standing in place
  */
void Animation::turn(Ogre::Radian a_turning_speed)
{
    if (current_animation == animation::stand) {
        //equivalent rate of animation for turning
        Ogre::Real turning_rate = -10 * a_turning_speed.valueRadians();

        //use turning rate or moving rate for anim - whichever is faster
        if (rate > 0) {
            //when moving forward
            if (turning_rate > rate || turning_rate < -rate) {
                rate = turning_rate;
                stopped = false;
            }
        } else {
            //when moving backward
            if (-turning_rate < rate || -turning_rate > -rate) {
                rate = -turning_rate;
                stopped = false;
            }
        }
    }
}

/** @brief blends animations between different walking modes: stand, walk, run
  * @todo: deal with crouch and collisions
  * @todo: disable anim when not on screen
  * @return whether the current_animation == target_animation
  */
bool Animation::switchAnimations(Ogre::Real a_time_to_add)
{
    if (current_animation != target_animation || switching_progress > 0) {
        //get keyframe
        Ogre::Real animation_time = animations[current_animation][0]->getTimePosition();

        //check if the time passed contains the switch point
        bool crossed_switch1 = (animation_time >= switch_points[0] &&
                                animation_time - a_time_to_add <= switch_points[0]);
        bool crossed_switch2 = (animation_time >= switch_points[1] &&
                                animation_time - a_time_to_add <= switch_points[1]);

        //continue blending
        switching_progress += abs(a_time_to_add);

        //if either switch passed or time limit reached change animation
        if (crossed_switch1 || crossed_switch2 || switching_progress >= switching_time) {
            //disable old anims
            for (usint i = 0, for_size = animations[current_animation].size(); i < for_size; ++i) {
                animations[current_animation][i]->setWeight(0);
                animations[current_animation][i]->setEnabled(false);
            }

            //switch anims
            current_animation = target_animation;

            //animation switched so no need to continue blending
            switching_progress = 0;

            //turn the new anims on
            for (usint i = 0, for_size = animations[current_animation].size(); i < for_size; ++i) {
                animations[current_animation][i]->setEnabled(true);
                animations[current_animation][i]->setWeight(1);

                //pick up the anim where the last one left off
                animations[current_animation][i]->setTimePosition(animation_time);
            }
        } else {
            //apply wieghts to blend animations smoothly
            Ogre::Real anim_weight = switching_progress / switching_time;

            for (usint i = 0, for_size = animations[current_animation].size(); i < for_size; ++i) {
                animations[target_animation][i]->setEnabled(true);
                animations[current_animation][i]->setWeight(1 - anim_weight);
                animations[target_animation][i]->setWeight(anim_weight);
                animations[target_animation][i]->setTimePosition(animation_time);
            }

            return false; //still trying to switch animations
        }
    }
    return true; //switched animations or no need to
}

/** @brief progresses the animation
  */
void Animation::update(Ogre::Real a_dt)
{
    if (!stopped) {
        if (rate == 0) { //if trying to stop ignore the speed and keep animating
            //get keyframe
            Ogre::Real animation_time = animations[current_animation][0]->getTimePosition();

            //did it cross the standing point
            bool crossed_stand1 = (animation_time >= stand_points[0] &&
                                   animation_time - a_dt <= stand_points[0]);
            bool crossed_stand2 = (animation_time >= stand_points[1] &&
                                   animation_time - a_dt <= stand_points[1]);

            //check if it's already in stand anim and crossed the standing position
            if (switchAnimations(a_dt) && (target_animation == animation::stand)
                && (crossed_stand1 || crossed_stand2)) {
                //managed to stop
                stopped = true;

                //reset the stand anim
                for (usint i = 0, size = animations[current_animation].size(); i < size; ++i) {
                    animations[current_animation][i]->setTimePosition(0);
                }
            } else {
                //keep animating until you hit these points and are in anim stand
                for (usint i = 0, size = animations[current_animation].size(); i < size; ++i) {
                    animations[current_animation][i]->addTime(a_dt);
                }
            }
        } else {
            //if moving translate time passed to animation time
            Ogre::Real time_to_add = a_dt * rate;

            //add the time to animations
            for (usint i = 0, for_size = animations[current_animation].size(); i < for_size; ++i) {
                animations[current_animation][i]->addTime(time_to_add);
            }

            switchAnimations(time_to_add);
        }
    }
}
