//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "formation_factory.h"
#include "formation.h"
#include "faction.h"

//struct for stl to use for comparison against formation name
struct FormationEqualsName {
    FormationEqualsName (const string& name) : formation_name(name) { };

    //name being looked for
    string formation_name;

    //comparison for stl find_if
    bool operator() (Formation* formation) { return formation_name == formation->getName(); }
};

FormationFactory::FormationFactory()
{
  //TODO: read formation from a file?
}

FormationFactory::~FormationFactory()
{
    //dtor
}

/** @brief creates a formation and adds it to the list
  */
Formation* FormationFactory::createFormation(const string& name, Faction* a_faction)
{
    //create a new formation
    formations.push_back(new Formation(name));

    //formation join the faction
    a_faction->joinFaction(formations.back());

    return formations.back();
}

/** @brief get's the formation based on its name
  * TODO: deal with missing formation better
  */
Formation* FormationFactory::getFormation(const string& name)
{
   //find the formation if it's on the list
    list<Formation*>::iterator it = find_if(formations.begin(), formations.end(),
                                            FormationEqualsName(name));

    //return the formation if found
    if (it != formations.end()) {
        return (*it);

    } else {
        //otherwise create a fomration of that name and default to new mercs - this is wrong TODO
        return createFormation(name, new Faction(global_faction::mercenary));
    }
}
