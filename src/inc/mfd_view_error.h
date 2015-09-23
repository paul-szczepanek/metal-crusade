// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFD_VIEW_ERROR_H
#define MFD_VIEW_ERROR_H

#include "mfd_view.h"

class MFDViewError : public MFDView
{
public:
  MFDViewError(hud_part_design_t& a_hud_part_design);
  virtual ~MFDViewError() { };

  // main loop
  virtual void update(Ogre::Real a_dt);

  // custom activation to show the view even on one call - this view may be shared
  void activate(bool a_toggle);

private:
  Ogre::Real flashing_accumulator;

  // so that one call is enough to make it appear
  bool on_screen;
};

#endif // MFD_VIEW_ERROR_H
