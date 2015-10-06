// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game.h"
#include "crusader_ai.h"
#include "unit_factory.h"
#include "crusader.h"
#include "game_controller.h"
#include "nav_point.h"

// to save myself typing - everything here is fake and temp!!!

CrusaderAI::CrusaderAI(Crusader* a_self)
  : enemy(a_self), self(a_self), goal(NULL)
{
  // fake and temp
}

CrusaderAI::~CrusaderAI()
{
}

// temp!!
void CrusaderAI::setEnemy(Crusader* a_enemy)
{
  if (a_enemy == NULL) { a_enemy = self; }

  enemy = a_enemy;
}

void CrusaderAI::setGoal(NavPoint* a_goal)
{
  goal = a_goal;
}

// TEMPTEMPTEMPTEMP!!!!
void CrusaderAI::update(const Real a_dt)
{
  // temp!!
  if (enemy != self && active) {
    game_controller->ControlBlock.turn_to_pointer = true;

    // turn to enemy
    game_controller->setPointerPos(enemy->getXYZ());

    // stupid homing and cirlcing behaviour
    Vector3 direction_to_enemy = enemy->getXYZ() - self->getXYZ();

    // if it's at an angle that can hit the target
    bool hitting = (self->getDirection().dotProduct(direction_to_enemy) > 0.99);

    if (self->getSpeed() < 2) {
      game_controller->setThrottle(0.75);

      // you hit something, turn
      if (self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
        game_controller->setTurnSpeed(1.0);
      } else {
        game_controller->setTurnSpeed(-1.0);
      }
    } else {
      if (direction_to_enemy.length() < 40) {
        game_controller->setThrottle(0.75);
        // turn sideways to the enemy
        if (self->getDriveDirection().dotProduct(direction_to_enemy) > 0.1) {
          if (self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
            game_controller->setTurnSpeed(1.0);
          } else {
            game_controller->setTurnSpeed(-1.0);
          }

        } else if (hitting) { // if you can hit enemy just keep going
          game_controller->setTurnSpeed(0.0);

        } else { // keep circling
          if (self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
            game_controller->setTurnSpeed(-1.0);
          } else {
            game_controller->setTurnSpeed(1.0);
          }
        }
      } else { // if too far away home in on enemy
        game_controller->setThrottle(1);
        if (self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
          game_controller->setTurnSpeed(-1.0);

        } else {
          game_controller->setTurnSpeed(1.0);
        }
      }

      if (hitting) {
        // fire less often nasty as hell
        if (Ogre::Math::UnitRandom() > 0.99 && direction_to_enemy.length() < 80) {
          game_controller->ControlBlock.fire = true;

        } else {
          game_controller->ControlBlock.fire = false;
        }

      } else {
        game_controller->ControlBlock.fire = false;
        game_controller->setThrottle(0.25);
      }
    }

  } else if (goal != NULL && active) { // temp!!!!!
    game_controller->ControlBlock.turn_to_pointer = true;

    // turn to goal
    game_controller->setPointerPos(goal->getXYZ());

    // stupid homing
    Vector3 direction_to_goal = goal->getXYZ() - self->getXYZ();
    direction_to_goal.normalise();

    // if it's at an angle that can hit the target
    bool hitting = (self->getDriveDirection().dotProduct(direction_to_goal) > 0.99);

    if (self->getSpeed() < 2) {
      game_controller->setThrottle(0.75);

      // you hit something, turn
      if (self->getDriveDirection().crossProduct(direction_to_goal).y > 0) {
        game_controller->setTurnSpeed(1.0);
      } else {
        game_controller->setTurnSpeed(-1.0);
      }
    } else { // home in
      if (hitting) {
        // if on target half speed ahead so others can catch up
        game_controller->setThrottle(0.45);
        game_controller->setTurnSpeed(0.0);

      } else {
        game_controller->setThrottle(0.7);

        if (self->getDriveDirection().crossProduct(direction_to_goal).y > 0) {
          game_controller->setTurnSpeed(-1.0);

        } else {
          game_controller->setTurnSpeed(1.0);
        }
      }
    }
  }
}
