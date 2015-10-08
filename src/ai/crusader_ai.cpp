// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game.h"
#include "crusader_ai.h"
#include "unit_factory.h"
#include "crusader.h"
#include "game_controller.h"
#include "nav_point.h"

// to save myself typing - everything here is fake and temp!!!

CrusaderAI::CrusaderAI(Crusader* a_self)
  : Enemy(a_self), Self(a_self), Goal(NULL)
{
  // fake and temp
}

CrusaderAI::~CrusaderAI()
{
}

// temp!!
void CrusaderAI::setEnemy(Crusader* a_enemy)
{
  if (a_enemy == NULL) { a_enemy = Self; }

  Enemy = a_enemy;
}

void CrusaderAI::setGoal(NavPoint* a_goal)
{
  Goal = a_goal;
}

// TEMPTEMPTEMPTEMP!!!!
void CrusaderAI::update(const Real a_dt)
{
  // temp!!
  if (Enemy != Self && Active) {
    Controller->ControlBlock.turn_to_pointer = true;

    // turn to enemy
    Controller->setPointerPos(Enemy->getXYZ());

    // stupid homing and cirlcing behaviour
    Vector3 direction_to_enemy = Enemy->getXYZ() - Self->getXYZ();

    // if it's at an angle that can hit the target
    bool hitting = (Self->getDirection().dotProduct(direction_to_enemy) > 0.99);

    if (Self->getSpeed() < 2) {
      Controller->setThrottle(0.75);

      // you hit something, turn
      if (Self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
        Controller->setTurnSpeed(1.0);
      } else {
        Controller->setTurnSpeed(-1.0);
      }
    } else {
      if (direction_to_enemy.length() < 40) {
        Controller->setThrottle(0.75);
        // turn sideways to the enemy
        if (Self->getDriveDirection().dotProduct(direction_to_enemy) > 0.1) {
          if (Self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
            Controller->setTurnSpeed(1.0);
          } else {
            Controller->setTurnSpeed(-1.0);
          }

        } else if (hitting) { // if you can hit enemy just keep going
          Controller->setTurnSpeed(0.0);

        } else { // keep circling
          if (Self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
            Controller->setTurnSpeed(-1.0);
          } else {
            Controller->setTurnSpeed(1.0);
          }
        }
      } else { // if too far away home in on enemy
        Controller->setThrottle(1);
        if (Self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
          Controller->setTurnSpeed(-1.0);

        } else {
          Controller->setTurnSpeed(1.0);
        }
      }

      if (hitting) {
        // fire less often nasty as hell
        if (Ogre::Math::UnitRandom() > 0.99 && direction_to_enemy.length() < 80) {
          Controller->ControlBlock.fire = true;

        } else {
          Controller->ControlBlock.fire = false;
        }

      } else {
        Controller->ControlBlock.fire = false;
        Controller->setThrottle(0.25);
      }
    }

  } else if (Goal != NULL && Active) { // temp!!!!!
    Controller->ControlBlock.turn_to_pointer = true;

    // turn to goal
    Controller->setPointerPos(Goal->getXYZ());

    // stupid homing
    Vector3 direction_to_goal = Goal->getXYZ() - Self->getXYZ();
    direction_to_goal.normalise();

    // if it's at an angle that can hit the target
    bool hitting = (Self->getDriveDirection().dotProduct(direction_to_goal) > 0.99);

    if (Self->getSpeed() < 2) {
      Controller->setThrottle(0.75);

      // you hit something, turn
      if (Self->getDriveDirection().crossProduct(direction_to_goal).y > 0) {
        Controller->setTurnSpeed(1.0);
      } else {
        Controller->setTurnSpeed(-1.0);
      }
    } else { // home in
      if (hitting) {
        // if on target half speed ahead so others can catch up
        Controller->setThrottle(0.45);
        Controller->setTurnSpeed(0.0);

      } else {
        Controller->setThrottle(0.7);

        if (Self->getDriveDirection().crossProduct(direction_to_goal).y > 0) {
          Controller->setTurnSpeed(-1.0);

        } else {
          Controller->setTurnSpeed(1.0);
        }
      }
    }
  }
}
