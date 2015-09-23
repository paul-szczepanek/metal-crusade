// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "mfd_computer.h"
#include "game.h"
#include "hud.h"
#include "game_controller.h"
#include "text_store.h"

MFDComputer::MFDComputer()
  : selected(false), selected_page(0), selecting_view(false), selected_view_index(0),
  num_of_views(Game::hud->hud_design.mfd_views.size())
{
  // init the mfd aux lines
  for (usint i = 0; i < hud_num_of_colours; ++i) {
    for (usint j = 0; j < hud_num_of_mfd_aux_lines; ++j) {
      for (usint k = 0; k < hud_mfd_aux_line_length; ++k) {
        mfd_aux_lines[i][j][k] = ' ';
      }
      // stick the string terminator in the last char
      mfd_aux_lines[i][j][hud_mfd_aux_line_length] = '\0';
    }
  }

  // set initial view
  if (num_of_views < 1) {
    Game::kill("No MFD views defined yet and an mfd exists - how do you expect that to work!?");
  }

  // each mfd gets different starting view
  if (Game::hud->getMFDNumber() < Game::hud->hud_design.mfd_views.size()) {
    selected_view = Game::hud->hud_design.mfd_views[Game::hud->getMFDNumber()];
  } else {
    selected_view = Game::hud->hud_design.mfd_views[0];
  }
}

/** @brief marks the mfd as selected so that it starts processing input
 */
void MFDComputer::activate(bool a_toggle)
{
  selected = a_toggle;

  // reset the state if deselected
  if (!selected) {
    selecting_view = false;
    take(Game::hud->controller->control_block.mfd_toggle);
  }
}

/** @brief select the mfd view
 */
void MFDComputer::selectView()
{
  // change page of views - PgUp, PgDn
  if (take(Game::hud->controller->control_block.mfd_left)) {
    if (selected_view_index > view_types_per_page - 1) {
      // previous page
      selected_view_index -= view_types_per_page;
      selected_view = Game::hud->hud_design.mfd_views[selected_view_index];
    }
  } else if (take(Game::hud->controller->control_block.mfd_right)) {
    if (selected_view < num_of_views - view_types_per_page) {
      // next page page
      selected_view_index += view_types_per_page;
      selected_view = Game::hud->hud_design.mfd_views[selected_view_index];
    }
  }

  // select views - up, down
  if (take(Game::hud->controller->control_block.mfd_up)) {
    if (selected_view_index > 0) {
      // previous view
      selected_view = Game::hud->hud_design.mfd_views[--selected_view_index];
    }
  } else if (take(Game::hud->controller->control_block.mfd_down)) {
    if (selected_view_index < num_of_views - 1) {
      // next view
      selected_view = Game::hud->hud_design.mfd_views[++selected_view_index];
    }
  }

  // show the page containing the selected view
  selected_page = selected_view_index / view_types_per_page;

  string page = intoString(selected_page + 1)
                + "/" + intoString(num_of_views / view_types_per_page);

  // set the label at the top:
  setLine(string("$a") + Game::text->getText(internal_string::select_view_type) + " " + page, 0);

  // set the lines with the names of view types - watch out, starts at 1
  for (usint i = 1; i < hud_num_of_mfd_aux_lines; ++i) {
    // get line numbers on this page
    usint listed = selected_page * view_types_per_page + (i - 1);

    // if there are enough view types
    if (listed < num_of_views) {
      if (listed == selected_view_index) {
        // mark the selected one in red
        setLine(string("$e") + Game::text->getText(mfd_view::view_names[listed]), i);

      } else {
        // print others in regular font
        setLine(string("$r") + Game::text->getText(mfd_view::view_names[listed]), i);
      }
    } else {
      // otherwise blank the line
      setLine("", i);
    }
  }

  // close the selecting mode if toggle pressed again
  if (take(Game::hud->controller->control_block.mfd_toggle)) {
    selecting_view = false;
  }
}

/** @brief main loop because MFDComputer needs input
 */
void MFDComputer::update(Real a_dt)
{
  if (selecting_view) {
    // mfd aux special view for selecting views for the mfd
    selectView();

  } else {
    if (selected) {
      // take input if selected
      if (take(Game::hud->controller->control_block.mfd_toggle)) {
        selecting_view = true;
      }

      // set top line as the label showing selected mfd view name
      setLine(string("$e") + Game::text->getText(mfd_view::view_names[selected_view]), 0);

    } else {
      // set top line as the label showing selected mfd view name
      setLine(string("$r") + Game::text->getText(mfd_view::view_names[selected_view]), 0);
    }

    // mfd aux directed by the MFD
    if (mfd_view::damage_diagram_self) {

    }

    // temp
    for (usint i = 1; i < hud_num_of_mfd_aux_lines; ++i) {
      setLine("", i);
    }
  }
}

/** @brief parse colours and set the lines
 */
void MFDComputer::setLine(const string& a_message,
                          usint         a_line)
{
  if (a_line < hud_num_of_mfd_aux_lines) {
    // fill the lines with monocolour strings
    Game::hud->parseColours(a_message, hud_mfd_aux_line_length, mfd_aux_lines[0][a_line],
                            mfd_aux_lines[1][a_line], mfd_aux_lines[2][a_line]);
  }
}
