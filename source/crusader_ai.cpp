//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "game.h"
#include "crusader_ai.h"
#include "unit_factory.h"
#include "crusader.h"
#include "game_controller.h"

CrusaderAI::CrusaderAI(Crusader* a_self)
{
    enemy = a_self;
    self = a_self;
    //fake and temp
    enemy = Game::unit_factory->getUnit(0);
}

void CrusaderAI::update()
{
    //temp!!
    if (enemy != self && active) {
        game_controller->control_block.turn_to_pointer = true;

        //turn to enemy
        game_controller->setPointerPos(enemy->getPosition());

        //stupid homing and cirlcing behaviour
        Ogre::Vector3 direction_to_enemy = enemy->getPosition() - self->getPosition();

        //if it's at an angle that can hit the target
        bool hitting = (self->getDirection().dotProduct(direction_to_enemy) > 0.99);

        if (direction_to_enemy.length() < 40) {
            //turn sideways to the enemy
            if (self->getDriveDirection().dotProduct(direction_to_enemy) > 0.1) {
                if (self->getDriveDirection().crossProduct(direction_to_enemy).y > 0)
                    game_controller->setTurn(1.0);
                else
                    game_controller->setTurn(-1.0);

            } else if (hitting) { //if you can hit enemy just keep going
                game_controller->setTurn(0.0);

            } else { //keep circling
                if (self->getDriveDirection().crossProduct(direction_to_enemy).y > 0)
                    game_controller->setTurn(-1.0);
                else
                    game_controller->setTurn(1.0);
            }
        } else { //if too far away home in on enemy
            if (self->getDriveDirection().crossProduct(direction_to_enemy).y > 0) {
                game_controller->setTurn(-1.0);

            } else {
                game_controller->setTurn(1.0);
            }
        }

        if (hitting) {
            //fire less often nasty as fuck
            if (Ogre::Math::UnitRandom() > 0.99 && direction_to_enemy.length() < 80) {
                game_controller->control_block.fire = true;

            } else {
                game_controller->control_block.fire = false;
            }

            game_controller->setThrottle(1.0);

        } else {
            game_controller->control_block.fire = false;
            game_controller->setThrottle(0.25);
        }
    }
}
