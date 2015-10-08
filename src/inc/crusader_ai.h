// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CRUSADERAI_H
#define CRUSADERAI_H

class Crusader;
class Corpus;
class ArenaEntity;
class NavPoint;

#include "unit_ai.h"

class CrusaderAI
  : public UnitAI
{
public:
  CrusaderAI(Crusader* a_self);
  virtual ~CrusaderAI();

  void update(const Real a_dt);

  void setGoal(NavPoint* a_goal);
  void setEnemy(Crusader* a_enemy);

private:
  ArenaEntity* Enemy;
  Crusader* Self;
  NavPoint* Goal;
};

#endif // CRUSADERAI_H
