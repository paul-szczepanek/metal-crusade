//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "faction.h"
#include "formation.h"

Faction::Faction()
{
    //ctor
}

Faction::~Faction()
{
    //dtor
}

int Faction::getRelation(Formation* a_formation)
{
    //find the formation if it's on the list
    list<pair<Formation*, int> >::iterator it = find_if(formation_relations.begin(),
                                                    formation_relations.end(),
                                                    PairFirstEquals<Formation*, int>(a_formation));

    //return the relation to the formation if found
    if (it != formation_relations.end()) {
        return (*it).second;

    } else {
        //otherwise return the relation to the formation
        return getRelation(a_formation->getFaction()->faction);
    }
}
