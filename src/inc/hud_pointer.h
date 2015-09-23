// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef HUD_POINTER_H
#define HUD_POINTER_H

#include "hud_part.h"

class HudPointer : public HudPart
{
public:
  HudPointer(hud_part_design_t& a_hud_part_design);
  ~HudPointer() { };

  // main loop
  void update(Ogre::Real a_dt);

private:
  Ogre::TextureUnitState* pointer_texture;

  // pointer functions
  static Ogre::Radian getAngleDirection();
  static Ogre::Radian getAngleTorsoDirection();
  static Ogre::Radian getAngleCompass();
  static Ogre::Radian getAngleZero();
  // fucntion pointer bound to at creation
  Ogre::Radian (*getAngle)();
};

#endif // HUD_POINTER_H
