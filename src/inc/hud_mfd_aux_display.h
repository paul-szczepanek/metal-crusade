// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFD_AUX_DISPLAY_H
#define MFD_AUX_DISPLAY_H

#include "hud_part.h"

class MFDComputer;

class HudMFDAuxDisplay : public HudPart
{
public:
  HudMFDAuxDisplay(hud_part_design_t& a_hud_part_design);
  ~HudMFDAuxDisplay() { };

  // main loop
  void update(Ogre::Real a_dt);

private:
  usint font_size;

  // the controlling mfd
  MFDComputer* mfd;

  // for lower fps
  Ogre::Real hud_part_interval;
  Ogre::Real hud_part_accumulator;

  // lines of text
  Ogre::OverlayElement* mfd_aux_text_elements[hud_num_of_colours][hud_num_of_mfd_aux_lines];
};

#endif // MFD_AUX_DISPLAY_H
