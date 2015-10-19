// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef HUD_POINTER_H
#define HUD_POINTER_H

#include "hud_part.h"
#include "game_hud.h"

class HudPointer
  : public HudPart
{
public:
  HudPointer(hud_part_design_t& a_hud_part_design);
  ~HudPointer()
  {
  }

  void update(const Real a_dt);

private:
  Ogre::TextureUnitState* pointer_texture;

  // pointer functions
  static Radian getAngleDirection();
  static Radian getAngleTorsoDirection();
  static Radian getAngleCompass();
  static Radian getAngleZero();
  // fucntion pointer bound to at creation
  Radian (*getAngle)();
};

#endif // HUD_POINTER_H
