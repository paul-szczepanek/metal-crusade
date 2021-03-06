// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_set.h"
#include "game.h"
#include "game_controller.h"
#include "game_timer.h"
#include "radar_computer.h"

HudSet::HudSet(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), item_to_show(0)
{
  // read params from design
  if (a_hud_part_design.parameters.size() < 1) { // kill the game if too few params
    Game::kill(string("hud_part missing params: ") + a_hud_part_design.name);
  }
  usint items_in_set = a_hud_part_design.parameters[0]; // get number of items in set

  // create an overlay element for each item
  for (size_t i = 0; i < items_in_set; ++i) {
    string id = string("set_") + intoString(function) + "_" + intoString(i);
    items.push_back(createPanel(id, a_hud_part_design.name + "_" + intoString(i),
                                0, 0, size.first, size.second, Container));
  }

  // bind the appropriate function for getting the value to the function pointer
  if (function == hud_part_enum::radar_power) {
    getItemToShow = getRadarPower;
  } else if (function == hud_part_enum::clock_tick) {
    getItemToShow = getClockTick;
  } else if (function == hud_part_enum::weapon_group) {
    getItemToShow = getWeaponGroup;
  } else if (function == hud_part_enum::weapon_target) {
    getItemToShow = getWeaponTarget;
  } else if (function == hud_part_enum::weapon_auto) {
    getItemToShow = getWeaponAuto;
  } else { // not handled or not handled yet
    getItemToShow = getValueZero;
  }

  // initial state
  switchItems(item_to_show);
}

void HudSet::update(Real a_dt)
{
  // get item to show depending on funciton
  usint new_item_to_show = getItemToShow();

  // switch only when needed
  if (item_to_show != new_item_to_show) {
    switchItems(new_item_to_show);
  }
}

void HudSet::switchItems(usint a_item_to_show)
{
  item_to_show = a_item_to_show;

  // show only that item or no item if the item_to_show doesn't exist
  for (size_t i = 0, for_size = items.size(); i < for_size; ++i) {
    if (item_to_show == i) {
      items[i]->show();
    } else {
      items[i]->hide();
    }
  }
}

// functions to be used to get the item from the set to show
// bound to at creation to save on constant checking every frame
usint HudSet::getRadarPower()
{
  if (Game::Hud->radar->getActive()) {
    if (Game::Hud->radar->getActiveRadar()) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 2;
  }
}

usint HudSet::getClockTick()
{
  if (Game::Timer->getTicks() % 1000 < 500) {
    return 1;
  } else {
    return 2;
  }
}

usint HudSet::getWeaponGroup()
{
  if (Game::Hud->Controller->ControlBlock.fire_mode_group) {
    return 0;
  } else {
    return 1;
  }
}

usint HudSet::getWeaponTarget()
{
  if (Game::Hud->Controller->ControlBlock.target_high) {
    return 1;
  } else if (Game::Hud->Controller->ControlBlock.target_low) {
    return 0;
  } else if (Game::Hud->Controller->ControlBlock.target_air) {
    return 2;
  } else {
    return 3;
  }
}

usint HudSet::getWeaponAuto()
{
  if (Game::Hud->Controller->ControlBlock.auto_cycle) {
    return 0;
  } else {
    return 1;
  }
}

usint HudSet::getValueZero()
{
  return 0;
}
