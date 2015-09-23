// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FACTION_H
#define FACTION_H

#include "main.h"
#include "global_faction.h"

class Formation;

class Faction
{
public:
  Faction(const string&                 a_name,
          const global_faction::faction a_faction,
          const ulint                   a_uid);
  virtual ~Faction();

  // name
  string& getName() {
    return name;
  }

  void setName(const string a_name) {
    name = a_name;
  }

  void attack(Faction* a_faction,
              Real     a_damage);

  void setParent(Faction* a_faction) {
    parent = a_faction;
  }

  Faction* getParent() {
    return parent;
  }

  // which global faction this represents - there may be more than one with the same faction field!
  global_faction::faction faction_type;
  ulint uid;

private:
  string name;
  // members of the faction
  list<Formation*> formations;

  Faction* parent;

  // TODO: list of object lists belonging to the faction so that attacking a building will work

  // TODO: last contact time with other factions so we can do fading of relations

  // formation joining the faction
  void joinFaction(Formation* a_formation);
  void leaveFaction(Formation* a_formation);

  friend class Formation;
};

#endif // FACTION_H
