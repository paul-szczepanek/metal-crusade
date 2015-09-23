// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef LOG_PRINTER_H
#define LOG_PRINTER_H

#include "hud_part.h"

class HudLogPrinter
  : public HudPart
{
public:
  HudLogPrinter(hud_part_design_t& a_hud_part_design);
  ~HudLogPrinter() {
  }

  // main loop
  void update(Real a_dt);

private:
  usint font_size;

  // position of the paper and printing head
  uint printed_log_line;
  Real timeout;

  // overlay elements
  Ogre::OverlayElement* log_text_elements[hud_num_of_colours][hud_num_of_log_lines];
  Ogre::TextureUnitState* paper_texture;
  Ogre::OverlayElement* hud_log_head;
};

const Real line_print_time = 0.75;
const Real line_print_timeout = 1;

#endif // LOG_PRINTER_H
