// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "formation.h"
#include "faction.h"
#include "game_controller.h"

Formation::Formation(const string& a_name,
                     Faction*      a_faction)
  : name(a_name), faction(a_faction)
{
  faction->joinFaction(this);
}

Formation::~Formation()
{
  for (list<GameController*>::iterator it = controllers.begin(); it != controllers.end(); ++it) {
    (*it)->setFormation(NULL);
  }

  controllers.clear();

  faction->leaveFaction(this);
}

/** @brief a controller (which represents the unit's pilot) joins a formation
 * automatically leaves any old one
 */
void Formation::setFaction(Faction* a_faction)
{
  faction = a_faction;
  faction->leaveFaction(this);
  faction->joinFaction(this);
}

/** @brief a controller (which represents the unit's pilot) joins a formation
 * automatically leaves any old one
 */
void Formation::joinFormation(GameController* a_controller)
{
  // check old formation
  Formation* old_formation = a_controller->getFormation();

  // if it exists leave it
  if (old_formation != NULL) {
    old_formation->leaveFormation(a_controller);
  }

  // put the the controller in the new formation
  controllers.push_back(a_controller);
  // set the new formation in the controller
  a_controller->setFormation(this);
}

/** @brief a controller (which represents the unit's pilot) leaves the formation
 * (used mostly internally)
 */
void Formation::leaveFormation(GameController* a_controller)
{
  // take the unit out of the list
  controllers.remove(a_controller);
  // and set the unit's formation to none
  a_controller->setFormation(NULL);
}
