// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "mfd_computer.h"
#include "game.h"
#include "game_controller.h"
#include "text_store.h"

MFDComputer::MFDComputer()
  : NumOfViews(Game::Hud->hud_design.mfd_views.size())
{
  // init the mfd aux lines
  for (usint i = 0; i < HUD_NUM_OF_COLOURS; ++i) {
    for (usint j = 0; j < HUD_NUM_OF_MFD_AUX_LINES; ++j) {
      for (usint k = 0; k < HUD_MFD_AUX_LINE_LENGTH; ++k) {
        mfd_aux_lines[i][j][k] = ' ';
      }
      // stick the string terminator in the last char
      mfd_aux_lines[i][j][HUD_MFD_AUX_LINE_LENGTH] = '\0';
    }
  }

  // set initial view
  if (NumOfViews < 1) {
    Game::kill("No MFD views defined yet and an mfd exists - how do you expect that to work!?");
  }

  // each mfd gets different starting view
  if (Game::Hud->getMFDNumber() < Game::Hud->hud_design.mfd_views.size()) {
    SelectedView = Game::Hud->hud_design.mfd_views[Game::Hud->getMFDNumber()];
  } else {
    SelectedView = Game::Hud->hud_design.mfd_views[0];
  }
}

MFDComputer::~MFDComputer()
{
}

/** @brief marks the mfd as selected so that it starts processing input
 */
void MFDComputer::activate(bool a_toggle)
{
  Selected = a_toggle;

  // reset the state if deselected
  if (!Selected) {
    SelectingView = false;
    take(Game::Hud->Controller->ControlBlock.mfd_toggle);
  }
}

/** @brief select the mfd view
 */
void MFDComputer::selectView()
{
  // change page of views - PgUp, PgDn
  if (take(Game::Hud->Controller->ControlBlock.mfd_left)) {
    if (SelectedViewIdx > VIEW_TYPES_PER_PAGE - 1) {
      // previous page
      SelectedViewIdx -= VIEW_TYPES_PER_PAGE;
      SelectedView = Game::Hud->hud_design.mfd_views[SelectedViewIdx];
    }
  } else if (take(Game::Hud->Controller->ControlBlock.mfd_right)) {
    if (SelectedView < NumOfViews - VIEW_TYPES_PER_PAGE) {
      // next page page
      SelectedViewIdx += VIEW_TYPES_PER_PAGE;
      SelectedView = Game::Hud->hud_design.mfd_views[SelectedViewIdx];
    }
  }

  // select views - up, down
  if (take(Game::Hud->Controller->ControlBlock.mfd_up)) {
    if (SelectedViewIdx > 0) {
      // previous view
      SelectedView = Game::Hud->hud_design.mfd_views[--SelectedViewIdx];
    }
  } else if (take(Game::Hud->Controller->ControlBlock.mfd_down)) {
    if (SelectedViewIdx < NumOfViews - 1) {
      // next view
      SelectedView = Game::Hud->hud_design.mfd_views[++SelectedViewIdx];
    }
  }

  // show the page containing the selected view
  SelectedPage = SelectedViewIdx / VIEW_TYPES_PER_PAGE;

  string page = intoString(SelectedPage + 1)
                + "/" + intoString(NumOfViews / VIEW_TYPES_PER_PAGE);

  // set the label at the top:
  setLine(string("$a") + Game::Text->getText(internal_string::select_view_type) + " " + page, 0);

  // set the lines with the names of view types - watch out, starts at 1
  for (usint i = 1; i < HUD_NUM_OF_MFD_AUX_LINES; ++i) {
    // get line numbers on this page
    usint listed = SelectedPage * VIEW_TYPES_PER_PAGE + (i - 1);

    // if there are enough view types
    if (listed < NumOfViews) {
      if (listed == SelectedViewIdx) {
        // mark the selected one in red
        setLine(string("$e") + Game::Text->getText(mfd_view::view_names[listed]), i);

      } else {
        // print others in regular font
        setLine(string("$r") + Game::Text->getText(mfd_view::view_names[listed]), i);
      }
    } else {
      // otherwise blank the line
      setLine("", i);
    }
  }

  // close the selecting mode if toggle pressed again
  if (take(Game::Hud->Controller->ControlBlock.mfd_toggle)) {
    SelectingView = false;
  }
}

/** @brief main loop because MFDComputer needs input
 */
void MFDComputer::update(Real a_dt)
{
  if (SelectingView) {
    // mfd aux special view for selecting views for the mfd
    selectView();

  } else {
    if (Selected) {
      // take input if selected
      if (take(Game::Hud->Controller->ControlBlock.mfd_toggle)) {
        SelectingView = true;
      }

      // set top line as the label showing selected mfd view name
      setLine(string("$e") + Game::Text->getText(mfd_view::view_names[SelectedView]), 0);

    } else {
      // set top line as the label showing selected mfd view name
      setLine(string("$r") + Game::Text->getText(mfd_view::view_names[SelectedView]), 0);
    }

    // mfd aux directed by the MFD
    if (mfd_view::damage_diagram_self) {

    }

    // temp
    for (usint i = 1; i < HUD_NUM_OF_MFD_AUX_LINES; ++i) {
      setLine("", i);
    }
  }
}

/** @brief parse colours and set the lines
 */
void MFDComputer::setLine(const string& a_message,
                          usint         a_line)
{
  if (a_line < HUD_NUM_OF_MFD_AUX_LINES) {
    // fill the lines with monocolour strings
    Game::Hud->parseColours(a_message, HUD_MFD_AUX_LINE_LENGTH, mfd_aux_lines[0][a_line],
                            mfd_aux_lines[1][a_line], mfd_aux_lines[2][a_line]);
  }
}
