// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef HUD_DIAL_H
#define HUD_DIAL_H

#include "hud_part.h"
#include "game_hud.h"

// parameters
// (min_value,
// max_value,
// min_angle,
// max_angle,
// direction) // +1 is clockwise, -1 is counterclockwise

class HudDial
  : public HudPart
{
public:
  HudDial(hud_part_design_t& a_hud_part_design);
  ~HudDial()
  {
  }

  void update(const Real a_dt);

private:
  // dial specs
  Real min_value;
  Real max_value;
  Real min_angle;
  Real max_angle;
  Real value;
  Real angle;
  Ogre::TextureUnitState* dial_texture;

  // dial functions
  static Real getValueSpeed();
  static Real getValuePressure();
  static Real getValueCoolant();
  static Real getValueEngineTemperature();
  static Real getValueCoreTemperature();
  static Real getValueExternalTemperature();
  static Real getValueThrottle();
  static Real getValueZero();
  // fucntion pointer bound to at creation
  Real (*getValue)();
};

#endif // HUD_DIAL_H
