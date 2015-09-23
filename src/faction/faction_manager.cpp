// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "faction_manager.h"
#include "faction.h"
#include "text_store.h"
#include "internal_string.h"
#include "game.h"

ulint FactionManager::uid = 0; // unieque id for each object in the game

// struct for stl to use for comparison against formation name
struct FactionEqualsName {
  FactionEqualsName (const string& name) : faction_name(name) { };

  // name being looked for
  string faction_name;

  // comparison for stl find_if
  bool operator() (Faction* faction) { return faction_name == faction->getName(); }
};

FactionManager::FactionManager()
{
  // TODO: read formations from a file
}

FactionManager::~FactionManager()
{
  // dtor
}

/** @brief creates a faction and adds it to the list
  * if given existing name produces a unique one
  */
Faction* FactionManager::createFaction(const string& name, global_faction::faction a_faction)
{
  // guarantee name uniqueness
  string unique_name = name;

  // keep trying suffixes until the name is unique
  for (uint i = internal_string::suffix1; i < internal_string::suffix24; ++i) {
    if (getFaction(unique_name)) {
      unique_name = name + " "
                    + Game::Text->getText(static_cast<internal_string::string_index>(i));

    } else {
      break;
    }
  }

  // fallback, shouldn't really happen
  while (getFaction(unique_name)) {
    unique_name += "_";
  }

  factions.push_back(new Faction(unique_name, a_faction, ++uid));

  return factions.back();
}

/** @brief get's the formation based on its name
  * TODO: deal with missing formation better
  */
Faction* FactionManager::getFaction(const string& name)
{
  // find the formation if it's on the list
  list<Faction*>::iterator it = find_if(factions.begin(), factions.end(),
                                        FactionEqualsName(name));

  // return the formation if found
  if (it != factions.end()) {
    return (*it);

  } else {
    // otherwise create a fomration of that name and default to new mercs - this is wrong TODO
    return NULL;
  }
}

/** @brief updates relations and if none exist creats them
  * returns the relation after adding the damage
  * crazy silly std way of mappings maps
  */
ulint FactionManager::attack(Faction* a_attacker, Faction* a_defender, Ogre::Real a_damage)
{
  map<ulint, map<ulint, Ogre::Real> >::iterator it = relations.find(a_defender->uid);

  // look for relations of this factions
  if (it != relations.end()) {
    map<ulint, Ogre::Real>::iterator it2 = it->second.find(a_attacker->uid);

    // if a relation to the attacker doesn't exist yet, create one
    if (it2 != it->second.end()) {
      it2->second += a_damage;

      updatePolicy(a_defender, a_attacker);

      return it2->second;

    } else {
      it->second.insert(pair<ulint, Ogre::Real>(a_attacker->uid, a_damage));
    }
  } else { // no relations exist yet
    // create the first one
    map<ulint, Ogre::Real> relation;
    relation.insert(pair<ulint, Ogre::Real>(a_attacker->uid, a_damage));

    // crea the map for relations and insert the relation
    relations.insert(pair<ulint, map<ulint, Ogre::Real> >(a_defender->uid, relation));
  }

  updatePolicy(a_defender, a_attacker);

  return a_damage;
}

Ogre::Real FactionManager::getRelation(Faction* a_from, Faction* a_to)
{
  map<ulint, map<ulint, Ogre::Real> >::iterator it = relations.find(a_from->uid);

  if (it != relations.end()) {
    map<ulint, Ogre::Real>::iterator it2 = it->second.find(a_to->uid);

    if (it2 != it->second.end()) {
      return it2->second;
    }
  }
  // TODO: return global relation
  return 0;
}

void FactionManager::Update(float delta)
{
  // this will update policies when needed with cooldowns
}

/** @brief creates a policy from relations that can be acted upon without evaluting every frame
  * this is really expensive and should be reevalutated very rarely
  */
void FactionManager::updatePolicy(Faction* a_from, Faction* a_to)
{
  // TODO: cooldown so policy is not re-examined to often
  // remember to register a update is required even if during cooldown so that another update
  // will happen at the end of cooldown - tricky, need a cron or an update func

  // start with direct relation
  Ogre::Real common_relation = getRelation(a_from, a_to);

  Faction* parent = a_from->getParent();

  // cap the relation based on parent felations
  while (parent != NULL) {
    Ogre::Real relation = getRelation(parent, a_to);

    // half going up so the top relation accounts for 1/2 of the relation used for policy
    // this could be changed to be arbitrary decentralisation value
    common_relation = 0.5 * common_relation + 0.5 * relation;

    parent = parent->getParent();
  }
}
