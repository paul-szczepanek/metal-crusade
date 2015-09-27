// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MFD_VIEW_DAMAGE_DIAGRAM_H
#define MFD_VIEW_DAMAGE_DIAGRAM_H

#include "mfd_view.h"

class Corpus;

class MFDViewDamageDiagram
  : public MFDView
{
public:
  MFDViewDamageDiagram(hud_part_design_t& a_hud_part_design);
  virtual ~MFDViewDamageDiagram()
  {
  }

  virtual void update(Real a_dt);

protected:
  // inner main loop
  void updateDiagramElements(Real    a_dt,
                             Corpus* a_object);

  // change diagram type to fit the target
  void switchDiagrams(mfd_view::diagram_type a_new_diagram);

  // diagram_type
  mfd_view::diagram_type diagram;

private:
  // creates the texture unit states for use with alpha blending control
  void createDiagram(mfd_view::diagram_type           a_diagram_type,
                     const vector<string>&            grn_names,
                     const vector<string>&            red_names,
                     vector<Ogre::TextureUnitState*>& materials_grn,
                     vector<Ogre::TextureUnitState*>& materials_red);

  // all the diagram vectors - used to enable and disable en masse
  vector<Ogre::OverlayContainer*> diagrams;
  Ogre::OverlayContainer* icons;
  Ogre::OverlayContainer* heat_gauge;

  // meterials for object pic
  vector<Ogre::TextureUnitState*> material_blank_grns;
  vector<Ogre::TextureUnitState*> material_blank_reds;

  // meterials for object pic
  vector<Ogre::TextureUnitState*> material_object_grns;
  vector<Ogre::TextureUnitState*> material_object_reds;

  // meterials for crusader pic
  vector<Ogre::TextureUnitState*> material_biped_crusader_grns;
  vector<Ogre::TextureUnitState*> material_biped_crusader_reds;

  // current materials
  vector<vector<Ogre::TextureUnitState*>*> materials_grns;
  vector<vector<Ogre::TextureUnitState*>*> materials_reds;

  // heat bar material
  Ogre::TextureUnitState* heat_bar_material;

  // icon overlay elements
  Ogre::OverlayElement* heat;
  Ogre::OverlayElement* heat_bar;
  pair<Ogre::OverlayElement*, Ogre::OverlayElement*> ico_engine;
  pair<Ogre::OverlayElement*, Ogre::OverlayElement*> ico_lock_in;
  pair<Ogre::OverlayElement*, Ogre::OverlayElement*> ico_electronics;

  // flashing
  vector<Real> flashing_accumulator;

  // bits to help position the pics - should be read from files really
  int offset_x;
  int ico_offset_y;
};

#endif // MFD_VIEW_DAMAGE_DIAGRAM_H
