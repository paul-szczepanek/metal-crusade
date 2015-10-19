#ifndef DYNAMICENTITY_H
#define DYNAMICENTITY_H

#include "main.h"
#include "arena_entity.h"
#include "corpus.h"

class Corpus;

class DynamicEntity
  : public ArenaEntity
{
public:
  DynamicEntity();
  virtual ~DynamicEntity();

  virtual Corpus* getGround();
  virtual void updateGround();

public:
  Corpus Ground;
};

#endif // DYNAMICENTITY_H

