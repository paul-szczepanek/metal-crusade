// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H

#include "hud_part.h"

class HudRadarDisplay
  : public HudPart
{
public:
  HudRadarDisplay(hud_part_design_t& a_hud_part_design);
  ~HudRadarDisplay();

  void update(Real a_dt);

private:
  // radar on/off
  bool active;

  // current radar range
  usint range_index;

  // dot materials
  Ogre::MaterialPtr dot_red;
  Ogre::MaterialPtr dot_green;

  // view damage
  vector<Ogre::OverlayElement*> dot_elements;
};

#endif // RADAR_DISPLAY_H
