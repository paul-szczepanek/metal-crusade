//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef FORMATION_H
#define FORMATION_H

#include "main.h"

class Faction;

class Unit;

class Formation
{
public:
    Formation(const string& a_name);
    virtual ~Formation();

    Faction* getFaction() { return faction; };
    string& getName() { return name; };

private:
    Formation* parent;
    list<Formation*> children;

    string name;
    Faction* faction;
    list<Unit*> units;
};

#endif // FORMATION_H
