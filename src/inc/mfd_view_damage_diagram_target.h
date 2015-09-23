// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFD_VIEW_DAMAGE_DIAGRAM_TARGET_H
#define MFD_VIEW_DAMAGE_DIAGRAM_TARGET_H

#include "mfd_view_damage_diagram.h"

class MFDViewDamageDiagramTarget : public MFDViewDamageDiagram
{
public:
  MFDViewDamageDiagramTarget(hud_part_design_t& a_hud_part_design);
  virtual ~MFDViewDamageDiagramTarget() { };

  // main loop
  void update(Ogre::Real a_dt);
};

#endif // MFD_VIEW_DAMAGE_DIAGRAM_TARGET_H
