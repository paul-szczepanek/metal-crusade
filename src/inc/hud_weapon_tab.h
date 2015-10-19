// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef WEAPON_TAB_H
#define WEAPON_TAB_H

#include "hud_part.h"
#include "game_hud.h"

class Weapon;
class GameController;

const usint number_of_lights = 8;

struct weapon_tab_lights_t {
  Ogre::OverlayElement* green[number_of_lights];
  Ogre::OverlayElement* yellow[number_of_lights];
  Ogre::OverlayElement* red[number_of_lights];
};

class HudWeaponTab
  : public HudPart
{
public:
  HudWeaponTab(hud_part_design_t& a_hud_part_design);
  ~HudWeaponTab()
  {
  }

  void update(const Real a_dt);

private:
  usint font_size;

  // for lower fps
  Real interval;
  Real accumulator;

  // alias for weapons
  vector<Weapon*> weapons;

  vector<Ogre::OverlayElement*> tab_name_elements;
  vector<Ogre::OverlayElement*> tab_ammo_elements;
  vector<weapon_tab_lights_t> tab_lights_elements;
};

#endif // WEAPON_TAB_H
