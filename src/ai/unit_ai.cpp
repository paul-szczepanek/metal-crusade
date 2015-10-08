// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "unit_ai.h"

UnitAI::UnitAI()
  : Active(false)
{
  // ctor
}

UnitAI::~UnitAI()
{
}

void UnitAI::bindController(GameController* a_game_controller)
{
  Controller = a_game_controller;
}

void UnitAI::activate(bool a_toggle)
{
  Active = a_toggle;
}
