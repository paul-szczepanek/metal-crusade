// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CRUSADERAI_H
#define CRUSADERAI_H

class Crusader;
class Corpus;
class Imaginarius;

#include "unit_ai.h"

class CrusaderAI : public UnitAI
{
public:
  CrusaderAI(Crusader* a_self);
  virtual ~CrusaderAI() { };

  // main loop
  void update();

  void setGoal(Imaginarius* a_goal);
  void setEnemy(Crusader* a_enemy);

private:
  Corpus* enemy;
  Crusader* self;
  Imaginarius* goal;
};

#endif // CRUSADERAI_H
