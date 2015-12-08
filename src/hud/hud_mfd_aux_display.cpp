// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_mfd_aux_display.h"
#include "mfd_computer.h"
#include "game.h"

HudMFDAuxDisplay::HudMFDAuxDisplay(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), hud_part_interval(0.1), hud_part_accumulator(0)
{
  // read parameters
  if (a_hud_part_design.parameters.size() < 1) { // kill the game if too few params
    Game::kill(string("hud_part missing param: ") + a_hud_part_design.name);
  }
  font_size = a_hud_part_design.parameters[0]; // read the size of the mfd aux font

  // line height for line positioning
  usint line_height = (size.second) / HUD_NUM_OF_MFD_AUX_LINES;

  // create the OGRE text elements to show the mfd aux lines
  for (size_t i = 0; i < HUD_NUM_OF_COLOURS; ++i) { // element for each colour
    for (size_t j = 0; j < HUD_NUM_OF_MFD_AUX_LINES; ++j) { // and each line
      string id = a_hud_part_design.name + "_" + intoString(i) + intoString(j);
      mfd_aux_text_elements[i][j]
        = createTextArea(id, "", font_size, Game::Hud->HudDesign.mfd_colours[i],
                         0, (j * line_height), size.first, size.second, Container);
    }
  }

  // get the mfd to hook up to
  Mfd = Game::Hud->getMFD();
}

void HudMFDAuxDisplay::update(Real a_dt)
{
  hud_part_accumulator += a_dt; // lower fps for digital parts
  if (hud_part_accumulator > hud_part_interval) { // use accumulator as dt
    // update all lines every time
    for (size_t i = 0; i < HUD_NUM_OF_COLOURS; ++i) {
      for (size_t j = 0; j < HUD_NUM_OF_MFD_AUX_LINES; ++j) {
        mfd_aux_text_elements[i][j]->setCaption(Mfd->getLine(i, j));
      }
    }

    hud_part_accumulator = 0;
  }
}
