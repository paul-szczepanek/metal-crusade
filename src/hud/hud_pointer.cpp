// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_pointer.h"
#include "game.h"
#include "unit.h"
#include "game_camera.h"

HudPointer::HudPointer(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design)
{
  // create texture for the pointer
  Ogre::MaterialPtr material = GameHud::createOverlayMaterial(a_hud_part_design.name);
  pointer_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);

  // apply it to the main container
  Container->setMaterialName(a_hud_part_design.name);

  // reposition the container to centre around x, y
  Container->setPosition(position.first - (size.first * 0.5),
                         position.second - (size.second * 0.5));

  // bind the appropriate function for getting the value to the function pointer
  if (function == hud_part_enum::direction) {
    getAngle = getAngleDirection;
  } else if (function == hud_part_enum::torso_direction) {
    getAngle = getAngleTorsoDirection;
  } else if (function == hud_part_enum::compass) {
    getAngle = getAngleCompass;
  } else { // not handled or not handled yet
    getAngle = getAngleZero;
  }
}

void HudPointer::update(Real a_dt)
{
  // rotate the pointer by rotating the texture UVs
  pointer_texture->setTextureRotate(getAngle());
}

// functions to be used to get the value for the dial
// bound to at creation to save on constant checking every frame
Radian HudPointer::getAngleDirection()
{
  return Game::Hud->PlayerUnit->getDriveOrientation().getYaw()
         + getAngleCompass();
}

Radian HudPointer::getAngleTorsoDirection()
{
  return Game::Hud->PlayerUnit->getOrientation().getYaw()
         + getAngleCompass();
}

Radian HudPointer::getAngleCompass()
{
  return Radian(pi) - Game::Camera->getOrientation().getYaw();
}

Radian HudPointer::getAngleZero()
{
  return Radian(0);
}
