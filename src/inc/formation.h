// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FORMATION_H
#define FORMATION_H

#include "main.h"

class Faction;
class GameController;

// this represents the group of actual units at a location
class Formation
{
public:
  Formation(const string& a_name,
            Faction*      a_faction);
  virtual ~Formation();

  // faction
  Faction* getFaction() const {
    return faction;
  }

  void setFaction(Faction* a_faction);

  // name
  string& getName() {
    return name;
  }

  void setName(const string a_name) {
    name = a_name;
  }

  // units joining and leaving the formation
  void joinFormation(GameController* a_controller);
  void leaveFormation(GameController* a_controller);

private:
  string name;
  Faction* faction;

  // members of the formation
  list<GameController*> controllers;
};

#endif // FORMATION_H
