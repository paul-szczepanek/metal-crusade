// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_mfd_display.h"
#include "mfd_computer.h"
#include "game.h"
#include "hud_mfd_display.h"
#include "mfd_view_damage_diagram.h"
#include "mfd_view_damage_diagram_target.h"
#include "mfd_view_error.h"

HudMFDDisplay::HudMFDDisplay(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), HudPartInterval(0.1), HudPartAccumulator(0)
{
  // read parameters
  if (a_hud_part_design.parameters.size() < 1) { // kill the game if too few params
    Game::kill(string("hud_part missing param: ") + a_hud_part_design.name);
  }
  FontSize = a_hud_part_design.parameters[0]; // read the size of the mfd aux font

  // get the mfd to hook up to
  Mfd = Game::Hud->getMFD();

  // a non-working (or not implented yet) view
  MFDView* view_error = new MFDViewError(a_hud_part_design);

  // create all the views
  for (size_t i = 0, for_size = Game::Hud->HudDesign.mfd_views.size(); i < for_size; ++i) {
    // create view according to types in the design
    if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::damage_diagram_self) {
      Views.push_back(new MFDViewDamageDiagram(a_hud_part_design));

    } else if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::damage_diagram_target) {
      Views.push_back(new MFDViewDamageDiagramTarget(a_hud_part_design));

    } else if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::damage_view_target) {
      Views.push_back(view_error); // temp!!

    } else if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::inspector) {
      Views.push_back(view_error); // temp!!

    } else if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::minimap) {
      Views.push_back(view_error); // temp!!

    } else if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::enemy_list) {
      Views.push_back(view_error); // temp!!

    } else if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::systems_control) {
      Views.push_back(view_error); // temp!!

    } else if (Game::Hud->HudDesign.mfd_views[i] == mfd_view::squad_tactic) {
      Views.push_back(view_error); // temp!!
    }
  }

  // set initial view
  CurrentView = Game::Hud->HudDesign.mfd_views[1];
  changeView(Game::Hud->HudDesign.mfd_views[CurrentView]);
}

HudMFDDisplay::~HudMFDDisplay()
{
  for (size_t i = 0, for_size = Views.size(); i < for_size; ++i) {
    delete Views[i];
  }
}

void HudMFDDisplay::update(Real a_dt)
{
  // use accumulator as dt
  HudPartAccumulator += a_dt;

  // lower fps for digital parts
  if (HudPartAccumulator > HudPartInterval) {
    // first update the mfd computer just before you poll it
    Mfd->update(HudPartAccumulator);

    // switch mfd views
    mfd_view::view_type new_view = Mfd->getViewType();
    if (new_view != CurrentView) {
      changeView(new_view);
    }

    // update the current view
    Views[CurrentView]->update(HudPartAccumulator);

    HudPartAccumulator = 0;
  }
}

void HudMFDDisplay::changeView(mfd_view::view_type a_view)
{
  Views[CurrentView]->activate(false);
  CurrentView = a_view;
  Views[CurrentView]->activate(true);

  // for (size_t i = 0, for_size = views.size(); i < for_size; ++i) {
  // activate selected view, deactivate all the rest
  //  views[i]->activate(current_view == i);
  // }
}
