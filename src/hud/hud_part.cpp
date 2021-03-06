// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_part.h"
#include "game.h"

HudPart::~HudPart()
{
  destroyOverlayContainer(Container);
}

HudPart::HudPart(hud_part_design_t& a_hud_part_design)
{
  // read the common design elements
  function = a_hud_part_design.function;
  position = a_hud_part_design.position;
  size = a_hud_part_design.size;

  // create the main container for the element
  Container = static_cast<Ogre::OverlayContainer*>
              (Game::Hud->OgreManager->createOverlayElement("Panel",
                                                            a_hud_part_design.name
                                                            + Game::getUniqueID()));

  // position and resize container
  Container->setMetricsMode(Ogre::GMM_PIXELS);
  Container->setPosition(position.first, position.second);
  Container->setDimensions(size.first, size.second);

  // add as a child to the main overlay
  Game::Hud->addContainer(a_hud_part_design.area, Container);
}

/** @brief simplify creation of elements, returns the element already added to the container
 */
Ogre::OverlayElement* HudPart::createPanel(const string&           a_id,
                                           const string&           a_material_name,
                                           int                     a_x,
                                           int                     a_y,
                                           int                     a_w,
                                           int                     a_h,
                                           Ogre::OverlayContainer* a_container)
{
  Ogre::OverlayElement* element
    = Game::Hud->OgreManager->createOverlayElement("Panel", a_id);

  // create and apply meterial
  GameHud::createOverlayMaterial(a_material_name);
  element->setMaterialName(a_material_name);

  // set dimensions
  element->setMetricsMode(Ogre::GMM_PIXELS);
  element->setPosition(a_x, a_y);
  element->setDimensions(a_w, a_h);

  // stick it in the container
  a_container->addChild(element);

  return element;
}

/** @brief simplify creation of text elements, returns the element already added to the container
 */
Ogre::OverlayElement* HudPart::createTextArea(const string&           a_id,
                                              const string&           a_text,
                                              int                     a_font_size,
                                              Ogre::ColourValue       a_colour,
                                              int                     a_x,
                                              int                     a_y,
                                              int                     a_w,
                                              int                     a_h,
                                              Ogre::OverlayContainer* a_container)
{
  Ogre::OverlayElement* element
    = Game::Hud->OgreManager->createOverlayElement("TextArea", a_id);

  // set dimensions
  element->setMetricsMode(Ogre::GMM_PIXELS);
  element->setPosition(a_x, a_y);
  element->setDimensions(a_w, a_h);

  // text elements
  element->setParameter("font_name", Game::Hud->HudDesign.font_name);
  element->setParameter("char_height", intoString(a_font_size));
  element->setColour(a_colour);
  element->setCaption(a_text);

  // stick it in the container
  a_container->addChild(element);

  return element;
}
