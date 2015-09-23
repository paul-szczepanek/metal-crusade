// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FACTIONFACTORY_H
#define FACTIONFACTORY_H

#include "main.h"
#include "global_faction.h"

class Formation;
class Faction;

class FactionManager
{
public:
  FactionManager();
  virtual ~FactionManager();

  void Update(float delta);

  Faction* createFaction(const string&           name,
                         global_faction::faction a_faction = global_faction::mercenary);

  Faction* getFaction(const string& name);

  // add or subrtact from relation
  ulint attack(Faction * a_attacker,
               Faction * a_defender,
               Real      a_damage);
  void updatePolicy(Faction* a_from,
                    Faction* a_to);

  Real getRelation(Faction* a_from,
                   Faction* a_to);

private:
  list<Faction*> factions;

  // everyone can be here
  map<ulint, map<ulint, Real> > relations;
  // only top factions need this
  map<ulint, map<global_faction::faction, Real> > global_relations;

  static ulint uid; // TODO: should reuse instead but tricky because of relations
};

#endif // FACTIONFACTORY_H
