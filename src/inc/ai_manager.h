// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef AI_FACTORY_H
#define AI_FACTORY_H

#include "main.h"

class UnitAI;
class CrusaderAI;
class Crusader;
class TankAI;
class Tank;
class Formation;
class GameController;

class AIManager
{
public:
  AIManager()
  {
  }

  ~AIManager();

  void update(const Real a_dt);

  // create ais for units
  CrusaderAI* activateUnit(Crusader*  a_unit,
                           Formation* a_formation);

  // ai creation
  CrusaderAI* createCrusaderAI(Crusader* a_self);
  TankAI* createCrusaderAI(Tank* a_self);

private:
  vector<UnitAI*> AIs;

  // controllers for the units on the map cotnrolled by ais
  vector<GameController*> AIGameControllers;
};

#endif // AI_FACTORY_H
