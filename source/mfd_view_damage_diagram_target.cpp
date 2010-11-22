//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "mfd_view_damage_diagram_target.h"
#include "game.h"
#include "hud.h"
#include "unit.h"

MFDViewDamageDiagramTarget::MFDViewDamageDiagramTarget(hud_part_design_t& a_hud_part_design)
    : MFDViewDamageDiagram(a_hud_part_design)
{
    //
}

void MFDViewDamageDiagramTarget::update(Ogre::Real a_dt)
{
    if (active) {
        if (Game::hud->player_unit->getTarget() == NULL) {
            if (diagram != mfd_view::blank) {
                switchDiagrams(mfd_view::blank);
            }
        } else {
            updateDiagramElements(a_dt, Game::hud->player_unit->getTarget());
        }
    }
}
