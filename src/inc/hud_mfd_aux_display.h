// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFD_AUX_DISPLAY_H
#define MFD_AUX_DISPLAY_H

#include "hud_part.h"
#include "game_hud.h"

class MFDComputer;

class HudMFDAuxDisplay
  : public HudPart
{
public:
  HudMFDAuxDisplay(hud_part_design_t& a_hud_part_design);
  ~HudMFDAuxDisplay()
  {
  }

  void update(const Real a_dt);

private:
  usint font_size;

  // the controlling mfd
  MFDComputer* mfd;

  // for lower fps
  Real hud_part_interval;
  Real hud_part_accumulator;

  // lines of text
  Ogre::OverlayElement* mfd_aux_text_elements[HUD_NUM_OF_COLOURS][HUD_NUM_OF_MFD_AUX_LINES];
};

#endif // MFD_AUX_DISPLAY_H
