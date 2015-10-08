// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "formation.h"
#include "faction.h"
#include "unit.h"

Formation::Formation(const string& a_name,
                     Faction*      a_faction)
  : Name(a_name), OwnerFaction(a_faction)
{
  OwnerFaction->joinFaction(this);
}

Formation::~Formation()
{
  for (auto u : Members) {
    u->setFormation(NULL);
  }

  Members.clear();

  OwnerFaction->leaveFaction(this);
}

/** @brief a controller (which represents the unit's pilot) joins a formation
 * automatically leaves any old one
 */
void Formation::setFaction(Faction* a_faction)
{
  OwnerFaction = a_faction;
  OwnerFaction->leaveFaction(this);
  OwnerFaction->joinFaction(this);
}

/** @brief a controller (which represents the unit's pilot) joins a formation
 * automatically leaves any old one
 */
void Formation::joinFormation(Unit* a_unit)
{
  // check old formation
  Formation* old_formation = a_unit->getFormation();

  // if it exists leave it
  if (old_formation != NULL) {
    old_formation->leaveFormation(a_unit);
  }

  // put the the controller in the new formation
  Members.push_back(a_unit);
  // set the new formation in the controller
  a_unit->setFormation(this);
}

/** @brief a controller (which represents the unit's pilot) leaves the formation
 * (used mostly internally)
 */
void Formation::leaveFormation(Unit* a_unit)
{
  // take the unit out of the list
  Members.remove(a_unit);
  // and set the unit's formation to none
  a_unit->setFormation(NULL);
}
