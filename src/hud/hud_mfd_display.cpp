// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_mfd_display.h"
#include "mfd_computer.h"
#include "game.h"
#include "hud_mfd_display.h"
#include "mfd_view_damage_diagram.h"
#include "mfd_view_damage_diagram_target.h"
#include "mfd_view_error.h"

HudMFDDisplay::HudMFDDisplay(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), hud_part_interval(0.1), hud_part_accumulator(0)
{
  // read parameters
  if (a_hud_part_design.parameters.size() < 1) { // kill the game if too few params
    Game::kill(string("hud_part missing param: ") + a_hud_part_design.name);
  }
  font_size = a_hud_part_design.parameters[0]; // read the size of the mfd aux font

  // get the mfd to hook up to
  mfd = Game::Hud->getMFD();

  // a non-working (or not implented yet) view
  MFDView* view_error = new MFDViewError(a_hud_part_design);

  // create all the views
  for (usint i = 0, for_size = Game::Hud->hud_design.mfd_views.size(); i < for_size; ++i) {
    // create view according to types in the design
    if (Game::Hud->hud_design.mfd_views[i] == mfd_view::damage_diagram_self) {
      views.push_back(new MFDViewDamageDiagram(a_hud_part_design));

    } else if (Game::Hud->hud_design.mfd_views[i] == mfd_view::damage_diagram_target) {
      views.push_back(new MFDViewDamageDiagramTarget(a_hud_part_design));

    } else if (Game::Hud->hud_design.mfd_views[i] == mfd_view::damage_view_target) {
      views.push_back(view_error); // temp!!

    } else if (Game::Hud->hud_design.mfd_views[i] == mfd_view::inspector) {
      views.push_back(view_error); // temp!!

    } else if (Game::Hud->hud_design.mfd_views[i] == mfd_view::minimap) {
      views.push_back(view_error); // temp!!

    } else if (Game::Hud->hud_design.mfd_views[i] == mfd_view::enemy_list) {
      views.push_back(view_error); // temp!!

    } else if (Game::Hud->hud_design.mfd_views[i] == mfd_view::systems_control) {
      views.push_back(view_error); // temp!!

    } else if (Game::Hud->hud_design.mfd_views[i] == mfd_view::squad_tactic) {
      views.push_back(view_error); // temp!!
    }
  }

  // set initial view
  current_view = Game::Hud->hud_design.mfd_views[1];
  changeView(Game::Hud->hud_design.mfd_views[current_view]);
}

HudMFDDisplay::~HudMFDDisplay()
{
  for (usint i = 0, for_size = views.size(); i < for_size; ++i) {
    delete views[i];
  }
}

void HudMFDDisplay::update(Real a_dt)
{
  // use accumulator as dt
  hud_part_accumulator += a_dt;

  // lower fps for digital parts
  if (hud_part_accumulator > hud_part_interval) {
    // first update the mfd computer just before you poll it
    mfd->update(hud_part_accumulator);

    // switch mfd views
    mfd_view::view_type new_view = mfd->getViewType();
    if (new_view != current_view) {
      changeView(new_view);
    }

    // update the current view
    views[current_view]->update(hud_part_accumulator);

    hud_part_accumulator = 0;
  }
}

void HudMFDDisplay::changeView(mfd_view::view_type a_view)
{
  views[current_view]->activate(false);
  current_view = a_view;
  views[current_view]->activate(true);

  // for (usint i = 0, for_size = views.size(); i < for_size; ++i) {
  // activate selected view, deactivate all the rest
  //  views[i]->activate(current_view == i);
  // }
}
