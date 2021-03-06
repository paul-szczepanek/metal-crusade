// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFDVIEW_H
#define MFDVIEW_H

#include "game_hud.h"
#include "hud_part.h"

class MFDComputer;

class MFDView
  : public HudPart
{
public:
  MFDView(hud_part_design_t& a_hud_part_design);
  virtual ~MFDView();

  virtual void update(const Real a_dt) = 0;

  // shows and hides the view and its overlay elements
  virtual void activate(bool a_toggle);

protected:
  // the controlling mfd
  MFDComputer* Mfd = NULL;
  bool Active = false;
};

#endif // MFDVIEW_H
