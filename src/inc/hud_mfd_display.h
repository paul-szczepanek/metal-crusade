// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFD_DISPLAY_H
#define MFD_DISPLAY_H

#include "hud_part.h"
#include "game_hud.h"

class MFDComputer;
class MFDView;

class HudMFDDisplay
  : public HudPart
{
public:
  HudMFDDisplay(hud_part_design_t& a_hud_part_design);
  ~HudMFDDisplay();

  void update(const Real a_dt);

private:
  void changeView(mfd_view::view_type a_view);

private:
  usint FontSize;

  // the controlling mfd
  MFDComputer* Mfd;

  // for lower fps
  Real HudPartInterval;
  Real HudPartAccumulator;

  // views
  vector<MFDView*> Views;
  mfd_view::view_type CurrentView;

  // view damage
  vector<Ogre::OverlayElement*> ViewDmgElements;
};

#endif // MFD_DISPLAY_H
