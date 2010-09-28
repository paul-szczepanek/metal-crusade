//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "formation_factory.h"
#include "formation.h"

//struct for stl to use for comparison against formation name
struct FormationEqualsName {
    FormationEqualsName (const string& name) : faction_name(name) { };

    //name being looked for
    string faction_name;

    //comparison for stl find_if
    bool operator() (Formation* faction) { return faction_name == faction->getName(); }
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
Formation* FormationFactory::createFormation(const string& name)
{
    formations.push_back(new Formation(name));
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

    //return the relation to the formation if found
    if (it != formations.end()) {
        return (*it);

    } else {
        //otherwise return the relation to the formation
        return createFormation("");
    }
}
