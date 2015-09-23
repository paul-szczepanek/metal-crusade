// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "faction.h"
#include "formation.h"
#include "game.h"
#include "faction_manager.h"

Faction::Faction(const string&                 a_name,
                 const global_faction::faction a_faction,
                 const ulint                   a_uid)
  : faction_type(a_faction), uid(a_uid), name(a_name)
{
  // ctor
}

Faction::~Faction()
{
  // dtor
}

void Faction::joinFaction(Formation* a_formation)
{
  formations.push_back(a_formation);
}

void Faction::leaveFaction(Formation* a_formation)
{
  formations.remove(a_formation);
}

void Faction::attack(Faction* a_faction,
                     Real     a_damage)
{
  Game::Faction->attack(this, a_faction, a_damage);
}
