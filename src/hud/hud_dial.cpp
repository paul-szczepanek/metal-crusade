// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_dial.h"
#include "game.h"
#include "unit.h"

HudDial::HudDial(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design)
{
  // read params from design
  if (a_hud_part_design.parameters.size() < 4) { // kill the game if too few params
    Game::kill(string("hud_part missing params: ") + a_hud_part_design.name);
  }
  min_value = a_hud_part_design.parameters[0];
  max_value = a_hud_part_design.parameters[1];
  min_angle = a_hud_part_design.parameters[2];
  max_angle = a_hud_part_design.parameters[3];

  // start with min_value
  value = min_value;

  // create texture for the dial
  Ogre::MaterialPtr material = GameHud::createOverlayMaterial(a_hud_part_design.name);
  dial_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);

  // apply it to the main container
  Container->setMaterialName(a_hud_part_design.name);
  // repostition to centre around x, y
  Container->setPosition(position.first - (size.first * 0.5),
                         position.second - (size.second * 0.5));

  // bind the appropriate function for getting the value to the funciton pointer
  if (function == hud_part_enum::speed) {
    getValue = getValueSpeed;
  } else if (function == hud_part_enum::pressure) {
    getValue = getValuePressure;
  } else if (function == hud_part_enum::coolant) {
    getValue = getValueCoolant;
  } else if (function == hud_part_enum::temperature_engine) {
    getValue = getValueEngineTemperature;
  } else if (function == hud_part_enum::temperature) {
    getValue = getValueCoreTemperature;
  } else if (function == hud_part_enum::temperature_external) {
    getValue = getValueExternalTemperature;
  } else if (function == hud_part_enum::throttle) {
    getValue = getValueThrottle;
  } else { // not handled or not handled yet
    getValue = getValueZero;
  }
}

void HudDial::update(Real a_dt)
{
  // get approriate value dependiong on dial function
  Real new_value = getValue();
  // smooth angle change // doesn't use dt to save on checking
  value = value * 0.9 + new_value * 0.1;

  // translate value to angle
  angle = min_angle + (max_angle - min_angle) * ((value - min_value) / (max_value - min_value));
  angle = min_angle + (max_angle - min_angle) * ((value - min_value) / (max_value - min_value));

  // rotate dial by rotating the texture UVs
  dial_texture->setTextureRotate(-1 * Radian(angle * (pi / 180)));
}

// functions to be used to get the value for the dial
// bound to at creation to save on constant checking every frame
Real HudDial::getValueSpeed()
{
  return Game::Hud->PlayerUnit->getSpeed() * 3.6; // convert to km/h
}

Real HudDial::getValuePressure()
{
  return Game::Hud->PlayerUnit->getPressure() * 100;
}

Real HudDial::getValueCoolant()
{
  return Game::Hud->PlayerUnit->getCoolant();
}

Real HudDial::getValueEngineTemperature()
{
  return Game::Hud->PlayerUnit->getEngineTemperature();
}

Real HudDial::getValueCoreTemperature()
{
  return Game::Hud->PlayerUnit->getCoreTemperature();
}

Real HudDial::getValueExternalTemperature()
{
  //return Game::Hud->player_unit->getSurfaceTemperature();
}

Real HudDial::getValueThrottle()
{
  return Game::Hud->PlayerUnit->getThrottle() * 3.6; // convert to km/h
}

Real HudDial::getValueZero()
{
  return 0;
}
