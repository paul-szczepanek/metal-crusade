//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef FORMATION_H
#define FORMATION_H

#include "main.h"

class Faction;
class GameController;

class Formation
{
public:
    Formation(const string& a_name);
    virtual ~Formation();

    //faction
    Faction* getFaction() { return faction; };
    void setFaction(Faction* a_faction) { faction = a_faction; };

    //name
    string& getName() { return name; };
    void setName(const string& a_name) { name = a_name; };

    //units joining and leaving the formation
    void joinFormation(GameController* a_controller);
    void leaveFormation(GameController* a_controller);

private:
    Formation* parent;
    list<Formation*> children;

    string name;
    Faction* faction;

    //members of the formation
    list<GameController*> controllers;
};

#endif // FORMATION_H
