// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "mfd_view_damage_diagram_target.h"
#include "game.h"
#include "unit.h"

MFDViewDamageDiagramTarget::MFDViewDamageDiagramTarget(hud_part_design_t& a_hud_part_design)
  : MFDViewDamageDiagram(a_hud_part_design)
{
  //
}

void MFDViewDamageDiagramTarget::update(Real a_dt)
{
  if (Active) {
    if (Game::Hud->PlayerUnit->getTarget() == NULL) {
      if (diagram != mfd_view::blank) {
        switchDiagrams(mfd_view::blank);
      }
    } else {
      updateDiagramElements(a_dt, Game::Hud->PlayerUnit->getTarget());
    }
  }
}
