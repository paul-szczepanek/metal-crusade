//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "faction.h"
#include "formation.h"

Faction::Faction(global_faction::faction a_faction) : faction(a_faction)
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

void Faction::setRelation(Formation* a_formation, int a_relation)
{
    //find the formation if it's on the list
    list<pair<Formation*, int> >::iterator it = find_if(formation_relations.begin(),
                                                    formation_relations.end(),
                                                    PairFirstEquals<Formation*, int>(a_formation));

    //amend the relation if it's already on the list of individual relations
    if (it != formation_relations.end()) {
        (*it).second = a_relation;

    } else {
        //push the formation onto the list of individual relations and set its relation
        formation_relations.push_back(make_pair(a_formation, a_relation));
    }
}

void Faction::joinFaction(Formation* a_formation)
{
    //leave old faction
    Faction* old_faction = a_formation->getFaction();
    if (old_faction != NULL) {
        old_faction->leaveFaction(a_formation);
    }

    //join the new one
    formations.push_back(a_formation);
    a_formation->setFaction(this);
}
