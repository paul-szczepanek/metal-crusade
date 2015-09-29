// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "unit_ai.h"

UnitAI::UnitAI()
  : active(false)
{
  // ctor
}

UnitAI::~UnitAI()
{
}

void UnitAI::bindController(GameController* a_game_controller)
{
  game_controller = a_game_controller;
}

void UnitAI::activate(bool a_toggle)
{
  active = a_toggle;
}
