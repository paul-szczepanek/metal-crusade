// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "mfd_view_error.h"
#include "game.h"

const int error_pic_w = 128;
const Real flashing_interval = 0.5;

MFDViewError::MFDViewError(hud_part_design_t& a_hud_part_design)
  : MFDView(a_hud_part_design), flashing_accumulator(0), on_screen(false)
{
  // al this view does is display the offline image
  string id = "mfd_red_offline";
  createPanel(id + Game::getUniqueID(), id, (size.first - error_pic_w) * 0.5,
              (size.second - error_pic_w) * 0.5, error_pic_w, error_pic_w, Container);

  // deactivate
  activate(false);
}

void MFDViewError::update(Real a_dt)
{
  if (Active) {
    // flash the img by alternative visble and not at flashing_interval
    flashing_accumulator += a_dt;

    // start of visible
    if (flashing_accumulator > 2 * flashing_interval) {
      flashing_accumulator = 0;
      Container->show();

    } else if (flashing_accumulator > flashing_interval) {
      // hide after flashing_interval
      Container->hide();
    }

    // reset each frame so it can be deactivated
    on_screen = false;
  }
}

/** @brief this activates it if only one positive call is made despite negative calls
 */
void MFDViewError::activate(bool a_toggle)
{
  if (on_screen == false) {
    // ignore unless it's not on screen yet
    Active = a_toggle;

    // if called as active set as on screen and ignore further calls this frame
    if (Active) {
      on_screen = true;
    }

    if (on_screen == false) {
      // hide when not even one call has been made
      Container->hide();

    } else {
      Container->show();
    }
  }
}
