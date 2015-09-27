// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FORMATIONFACTORY_H
#define FORMATIONFACTORY_H

#include "main.h"

class Formation;
class Faction;

class FormationManager
{
public:
  FormationManager();
  virtual ~FormationManager();

  void update(Real delta);

  Formation* createFormation(const string& name,
                             Faction*      a_faction);

  Formation* getFormation(const string& name);

private:
  list<Formation*> formations;
};

#endif // FORMATIONFACTORY_H
