#ifndef ENTITY_H
#define ENTITY_H

#include "main.h"

class Corpus;
class Unit;

class ArenaEntity
{
public:
  ArenaEntity();
  ArenaEntity(const string& a_name);
  virtual ~ArenaEntity();
  void setName(const string& a_name);
  void setOwner(Unit* a_owner);
  bool isFree();

public:
  string Name;
  Unit* Owner;
};

inline void ArenaEntity::setName(const string& a_name)
{
  Name = a_name;
}

inline void ArenaEntity::setOwner(Unit* a_owner);
{
  Owner = a_owner;
}

inline bool ArenaEntity::isFree();
{
  return Owner == NULL;
}

#endif // ENTITY_H
