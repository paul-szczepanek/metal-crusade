// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef HUDPART_H
#define HUDPART_H

#include "main.h"
#include "game_hud.h"
#include "hud_design.h"

#define HUD_3D_DISTANCE (-200)

class HudPart
{
public:
  HudPart(hud_part_design_t& a_hud_part_design);
  virtual ~HudPart();

  virtual void update(const Real a_dt) = 0;
  virtual void resize(Real a_scale);

protected:
  // basic common properties
  int_pair position;
  int_pair size;
  hud_part_enum::function function;

  // master container for all elements
  Ogre::OverlayContainer* Container;

  // short hand for creating elements
  Ogre::OverlayElement* createPanel(const string&           a_id,
                                    const string&           a_material_name,
                                    int                     a_x,
                                    int                     a_y,
                                    int                     a_w,
                                    int                     a_h,
                                    Ogre::OverlayContainer* a_container);
  Ogre::OverlayElement* createTextArea(const string&           a_id,
                                       const string&           a_text,
                                       int                     a_font_size,
                                       Ogre::ColourValue       a_colour,
                                       int                     a_x,
                                       int                     a_y,
                                       int                     a_w,
                                       int                     a_h,
                                       Ogre::OverlayContainer* a_container);
};

inline void HudPart::resize(Real a_scale)
{
}

#endif // HUDPART_H
