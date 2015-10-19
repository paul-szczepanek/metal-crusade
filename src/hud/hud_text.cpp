// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_text.h"
#include "radar_computer.h"
#include "game.h"
#include "unit.h"
#include "game_timer.h"

HudText::HudText(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), text("-"), hud_part_interval(0.1), hud_part_accumulator(0)
{
  // get font size
  if (a_hud_part_design.parameters.size() < 1) { // kill the game if too few params
    Game::kill(string("hud_part missing param: ") + a_hud_part_design.name);
  }
  font_size = a_hud_part_design.parameters[0]; // read the size of the status font

  // create the text element
  string id = a_hud_part_design.name + "_text_area_" + intoString(function);
  text_element = createTextArea(id, text, font_size, Game::Hud->HudDesign.display_colours[0],
                                0, 0, size.first, size.second, Container);

  // bind the appropriate function for getting the value to the function pointer
  if (function == hud_part_enum::clock) {
    getText = getTextClock;
  } else if (function == hud_part_enum::radar_range) {
    getText = getTextRadarRange;
  } else if (function == hud_part_enum::radar_power) {
    getText = getTextRadarPower;
  } else if (function == hud_part_enum::weapon_group) {
    getText = getTextWeaponGroup;
  } else if (function == hud_part_enum::weapon_target) {
    getText = getTextWeaponTarget;
  } else if (function == hud_part_enum::weapon_auto) {
    getText = getTextWeaponAuto;
  } else if (function == hud_part_enum::temperature) {
    getText = getTextTemperature;
  } else if (function == hud_part_enum::temperature_external) {
    getText = getTextTemperatureExternal;
  } else if (function == hud_part_enum::coolant) {
    getText = getTextCoolant;
  } else if (function == hud_part_enum::speed) {
    getText = getTextSpeed;
  } else if (function == hud_part_enum::throttle) {
    getText = getTextThrottle;
  } else if (function == hud_part_enum::pressure) {
    getText = getTextPressure;
  } else if (function == hud_part_enum::temperature_engine) {
    getText = getTextTemperatureEngine;
  } else { // not handled or not handled yet
    getText = getTextZero;
  }
}

void HudText::update(Real a_dt)
{
  // lower fps for digital parts
  hud_part_accumulator += a_dt;

  if (hud_part_accumulator > hud_part_interval) { // use accumulator as dt
    // set the text
    text = getText();
    text_element->setCaption(text);

    // change the colour if any colour marker exists (per line colour)
    if (text.length() > 2 && text[0] == HUD_ESCAPE_CHAR) { // if the string start with a $
      // assume default colour
      usint colour = 0;

      // start at 1 as 0 is default
      for (usint i = 1; i < HUD_NUM_OF_COLOURS; ++i) {
        if (text[1] == hud_colour_codes[i]) { // if a valid colour code
          colour = i; // use that colour instead
          break;
        }
      }

      // set the colour of the whole line
      text_element->setColour(Game::Hud->HudDesign.display_colours[colour]);
    }

    // reset accumulator for next frame
    hud_part_accumulator = 0;
  }
}

// functions to be used to get the value for the text
// bound to at creation to save on constant checking every frame

string HudText::getTextClock()
{
  // get time
  lint miliseconds = Game::Timer->getTicks();
  usint hours = miliseconds / 3600000;
  usint minutes = (miliseconds % 3600000) / 60000;
  usint seconds = (miliseconds % 60000) / 1000;

  // prepare a string in 12:34:34 format
  stringstream stream;
  string time_string;
  stream << setw(2) << setfill('0') << hours << ":";
  stream << setw(2) << minutes << ":";
  stream << setw(2) << seconds << endl;
  stream >> time_string;

  return time_string;
}

string HudText::getTextRadarRange()
{
  return realIntoString(Game::Hud->radar->getRadarRange(), 3);
}

string HudText::getTextRadarPower()
{
  return string("-");
}

string HudText::getTextWeaponGroup()
{
  return string("-");
}

string HudText::getTextWeaponTarget()
{
  return string("-");
}

string HudText::getTextWeaponAuto()
{
  return string("-");
}

string HudText::getTextTemperature()
{
  return string("-");
}

string HudText::getTextCoolant()
{
  return string("-");
}

string HudText::getTextSpeed()
{
  return string("-");
}

string HudText::getTextThrottle()
{
  return string("-");
}

string HudText::getTextPressure()
{
  return string("-");
}

string HudText::getTextTemperatureExternal()
{
  return string("-");
}

string HudText::getTextTemperatureEngine()
{
  return string("-");
}

string HudText::getTextZero()
{
  return string("-");
}
