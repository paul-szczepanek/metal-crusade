// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef HUD_TEXT_H
#define HUD_TEXT_H

#include "hud_part.h"
#include "game_hud.h"

class HudText
  : public HudPart
{
public:
  HudText(hud_part_design_t& a_hud_part_design);
  ~HudText()
  {
  }

  void update(const Real a_dt);

private:
  string text;
  Ogre::OverlayElement* text_element;
  usint font_size;

  // lower fps
  Real hud_part_interval;
  Real hud_part_accumulator;

  // text functions
  static string getTextClock();
  static string getTextRadarRange();
  static string getTextRadarPower();
  static string getTextWeaponGroup();
  static string getTextWeaponTarget();
  static string getTextWeaponAuto();
  static string getTextTemperature();
  static string getTextTemperatureExternal();
  static string getTextCoolant();
  static string getTextSpeed();
  static string getTextThrottle();
  static string getTextPressure();
  static string getTextTemperatureEngine();
  static string getTextZero();
  // fucntion pointer bound to at creation
  string (*getText)();
};

#endif // HUD_TEXT_H
