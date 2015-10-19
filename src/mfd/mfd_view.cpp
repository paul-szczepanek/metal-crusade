// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "mfd_view.h"
#include "game.h"

MFDView::MFDView(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design)
{
  // get the mfd to hook up to
  Mfd = Game::Hud->getMFD();
}

MFDView::~MFDView()
{
}

/** @brief hides the whole container when the view is not selected and vice versa
 */
void MFDView::activate(bool a_toggle)
{
  Active = a_toggle;
  if (Active) {
    Container->show();

  } else {
    // hide everything when deactivated
    Container->hide();
  }
}
