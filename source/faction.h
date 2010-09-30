//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef FACTION_H
#define FACTION_H

#include "main.h"
#include "pair_equals.h"
#include "global_faction.h"

class Formation;

class Faction
{
public:
    Faction(global_faction::faction a_faction);
    virtual ~Faction();

    //realtions with other factions
    int getRelation(global_faction::faction a_faction) { return faction_relations[a_faction]; };
    int getRelation(Formation* a_formation);
    void setRelation(global_faction::faction a_faction, int a_relation);
    void setRelation(Formation* a_formation, int a_relation);

    //formation joining the faction
    void joinFaction(Formation* a_formation);

    //which global faction this represents - there may be more than one with the same faction field!
    global_faction::faction faction;

private:
    //you can't leave a faction, you can only join a new one thus leave the old one
    void leaveFaction(Formation* a_formation) { formations.remove(a_formation); };

    //relations to factions and formations
    int faction_relations[global_faction::number];
    list<pair<Formation*, int> > formation_relations;

    //members of the faction
    list<Formation*> formations;
};

inline void Faction::setRelation(global_faction::faction a_faction, int a_relation)
{
    faction_relations[a_faction] = a_relation;
}

#endif // FACTION_H
