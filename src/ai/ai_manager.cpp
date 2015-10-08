// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "ai_manager.h"
#include "unit_ai.h"
#include "crusader_ai.h"
#include "game_controller.h"
#include "formation.h"
#include "game.h"
#include "crusader.h"

AIManager::~AIManager()
{
  // cleanup ais
  for (auto ai : AIs) {
    delete ai;
  }
  for (auto ai_con : AIGameControllers) {
    delete ai_con;
  }
}

/** @brief updates AIs
 * @todo: pretty much everything
 */
void AIManager::update(const Real a_dt)
{
  // call update on every unit
  for (size_t i = 0, for_size = AIs.size(); i < for_size; ++i) {
    AIs[i]->update(a_dt);
    // TODO give them group plans
  }
}

/** @brief creates ais and add's them to a list
 */
CrusaderAI* AIManager::createCrusaderAI(Crusader* a_self)
{
  CrusaderAI* ai = new CrusaderAI(a_self);

  // put the unit on the update list
  AIs.push_back(ai);

  // TODO put them in groups

  return ai;
}

/** @brief creates an AI and hooks it up to the crusader
 * @todo: add personality enum parameter
 */
CrusaderAI* AIManager::activateUnit(Crusader*  a_unit,
                                    Formation* a_formation)
{
  // create a new controller for the ai to use
  AIGameControllers.push_back(new GameController("crusader ai"));
  // join the formation
  a_formation->joinFormation(a_unit);

  // create the ai and give it the controller
  CrusaderAI* ai = Game::AI->createCrusaderAI(a_unit);
  // assign the controller to the ai
  ai->bindController(AIGameControllers.back());
  a_unit->assignController(AIGameControllers.back());
  ai->activate(true);

  return ai;
}
