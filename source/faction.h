//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef FACTION_H
#define FACTION_H

#include "main.h"
#include "pair_equals.h"
#include "global_faction.h"

class Formation;

class Faction
{
public:
    Faction();
    virtual ~Faction();

    int getRelation(global_faction::faction a_faction) { return faction_relations[a_faction]; };
    int getRelation(Formation* a_formation);

    //which faction this represents - there may be more than one with the same faction field!
    global_faction::faction faction;

private:
    //relations to factions and formations
    int faction_relations[global_faction::number];
    list<pair<Formation*, int> > formation_relations;
};

#endif // FACTION_H
