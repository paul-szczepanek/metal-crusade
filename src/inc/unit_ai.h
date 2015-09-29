// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef UNITAI_H
#define UNITAI_H

class GameController;

#include "main.h"

class UnitAI
{
public:
  UnitAI();
  virtual ~UnitAI();

  virtual void update(Real a_dt) = 0;

  virtual void activate(bool a_toggle);
  void bindController(GameController* a_game_controller);

protected:
  bool active;
  GameController* game_controller;
};

#endif // UNITAI_H
