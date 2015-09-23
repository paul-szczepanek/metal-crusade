// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "mfd_view.h"
#include "game.h"
#include "hud.h"

MFDView::MFDView(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), active(false)
{
  // get the mfd to hook up to
  mfd = Game::hud->getMFD();
}

/** @brief hides the whole container when the view is not selected and vice versa
  */
void MFDView::activate(bool a_toggle)
{
  active = a_toggle;
  if (active) {
    container->show();

  } else {
    // hide everything when deactivated
    container->hide();
  }
}
