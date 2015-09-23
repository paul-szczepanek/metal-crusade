#ifndef ENTITY_H
#define ENTITY_H

#include "main.h"

class Corpus;
class Unit;

class ArenaEntity
{
public:
  ArenaEntity(const string& a_name);
  virtual ~ArenaEntity() {};

private:
  string Name;
  Corpus* Object;
  Unit* Owner;
};

#endif // ENTITY_H
