// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "formation_manager.h"
#include "formation.h"
#include "text_store.h"
#include "internal_string.h"
#include "game.h"

// struct for stl to use for comparison against formation name
struct FormationEqualsName {
  FormationEqualsName (const string& name) : formation_name(name) { };

  // name being looked for
  string formation_name;

  // comparison for stl find_if
  bool operator() (Formation* formation) { return formation_name == formation->getName(); }
};

FormationManager::FormationManager()
{
  // ctor
}

FormationManager::~FormationManager()
{
  // dtor
}

void FormationManager::Update(float delta)
{
  // this will update formations
}

/** @brief creates a formation and adds it to the list
  */
Formation* FormationManager::createFormation(const string& name, Faction* a_faction)
{
  // guarantee name uniqueness
  string unique_name = name;

  // keep trying suffixes until the name is unique
  for (uint i = internal_string::suffix1; i < internal_string::suffix24; ++i) {
    if (getFormation(unique_name)) {
      unique_name = name + " "
                    + Game::Text->getText(static_cast<internal_string::string_index>(i));

    } else {
      break;
    }
  }

  // fallback, shouldn't really happen
  while (getFormation(unique_name)) {
    unique_name += "_";
  }

  formations.push_back(new Formation(unique_name, a_faction));

  return formations.back();
}

/** @brief get's the formation based on its name
  * TODO: deal with missing formation better
  */
Formation* FormationManager::getFormation(const string& name)
{
  // find the formation if it's on the list
  list<Formation*>::iterator it = find_if(formations.begin(), formations.end(),
                                          FormationEqualsName(name));

  // return the formation if found
  if (it != formations.end()) {
    return (*it);

  } else {
    return NULL;
  }
}
