// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H

#include "hud_part.h"
#include "game_hud.h"

class HudRadarDisplay
  : public HudPart
{
public:
  HudRadarDisplay(hud_part_design_t& a_hud_part_design);
  ~HudRadarDisplay();

  void update(const Real a_dt);

private:
  // radar on/off
  bool Enabled = false;

  // current radar range
  usint RangeIndex = 0;

  // dot materials
  Ogre::MaterialPtr RedMaterial;
  Ogre::MaterialPtr GreenMaterial;

  // view damage
  vector<Ogre::OverlayElement*> DotElements;
};

#endif // RADAR_DISPLAY_H
