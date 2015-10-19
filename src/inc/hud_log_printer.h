// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef LOG_PRINTER_H
#define LOG_PRINTER_H

#include "hud_part.h"
#include "game_hud.h"

class HudLogPrinter
  : public HudPart
{
public:
  HudLogPrinter(hud_part_design_t& a_hud_part_design);
  ~HudLogPrinter()
  {
  }

  void update(const Real a_dt);

private:
  usint font_size;

  // position of the paper and printing head
  size_t printed_log_line;
  Real timeout;

  // overlay elements
  Ogre::OverlayElement* log_text_elements[HUD_NUM_OF_COLOURS][HUD_NUM_OF_LOG_LINES];
  Ogre::TextureUnitState* paper_texture;
  Ogre::OverlayElement* hud_log_head;
};

const Real line_print_time = 0.75;
const Real line_print_timeout = 1;

#endif // LOG_PRINTER_H
