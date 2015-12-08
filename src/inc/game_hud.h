// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef HUD_H
#define HUD_H

#include "main.h"
#include "hud_design.h"
#ifdef PLATFORM_WIN32
#include <OgreOverlaySystem.h>
#else
#include <OGRE/Overlay/OgreOverlaySystem.h>
#endif

// hud designs in pixel coords baselined at 1024x768
#define BASE_HUD_WIDTH (1024)
#define BASE_HUD_HEIGHT (768)

class HudPart;
class Unit;
class GameController;
class Timer;
class MFDComputer;
class LogComputer;
class StatusComputer;
class RadarComputer;

namespace interface_mode
{
enum modes {
  mfd,
  log,
  computer,
  communication
};

};

class GameHud
{
public:
  GameHud();
  ~GameHud();
  // init
  void loadHud(Unit* a_player_unit);
  void activate(bool a_toggle);

  void update(const Real a_dt);

  // shows the pause screen
  void pause();

  // called by input manager when the window resizes
  void resize(size_t a_screen_width,
              size_t a_screen_height);

  // hookup for hud parts
  void addContainer(hud_area                a_hud_area,
                    Ogre::OverlayContainer* a_container);
  void addElement(Ogre::SceneNode* a_scene_node);

  // parsing colour codes in a string into monocolour lines
  void parseColours(const string& message,
                    usint         a_length,
                    char*         line1,
                    char*         line2,
                    char*         line3);

  // hooking up MFDs to their displays
  MFDComputer* getMFD()
  {
    return mfds.back();
  }

  size_t getMFDNumber()
  {
    return mfds.size();
  }

  // helper functions
  static Ogre::MaterialPtr createOverlayMaterial(
    const string&               a_name,
    texture_addressing          a_addressing
      = Ogre::TextureUnitState::TAM_CLAMP,
    string                      a_texture_name =
      string(""));
  Real getHudAreaOriginX(hud_area a_hud_area);
  Real getHudAreaOriginY(hud_area a_hud_area);

private:
  bool getHudDesign(const string& filename,
                    hud_design_t& HudDesign);
  void padHudColours(vector<Ogre::ColourValue>& colour_array);

  // positioning
  void offsetUpdate(Real     a_dt,
                    hud_area a_hud_area,
                    bool     a_alternative);
  Real positionHorizontal(int                  a_width,
                          horizontal::position a_position,
                          Real                 a_offset);
  Real positionVertical(int                a_height,
                        vertical::position a_position,
                        Real               a_offset);
public:
  Ogre::OverlayManager* OgreManager = NULL;
  // log and status display
  LogComputer* log = NULL;
  StatusComputer* status = NULL;

  // the unit using the hud
  Unit* PlayerUnit = NULL;
  GameController* Controller = NULL;
  RadarComputer* radar = NULL;

  // hud design
  hud_design_t HudDesign;
private:
  // hud areas
  Ogre::OverlayContainer* hud_areas[hud_area_MAX];
  Ogre::Overlay* hud_overlays[hud_area_MAX];
  Ogre::Overlay* hud_overlay_3d = NULL;

  // special temp containers for pause
  Ogre::OverlayContainer* centre_container = NULL;
  Ogre::Overlay* centre_overlay = NULL;

  // hud size and position
  bool Active = false;
  Real hud_width = BASE_HUD_WIDTH;
  Real hud_height = BASE_HUD_HEIGHT;
  Real Scale = 1;
  Real ScaleW = 1;
  Real ScaleH = 1;
  int_pair area_offsets[hud_area_MAX];

  // all the individual hud parts
  vector<HudPart*> hud_parts;

  // MFD
  vector<MFDComputer*> mfds;
  usint selected_mfd = 0;

  // wchich item is active
  interface_mode::modes hud_mode = interface_mode::mfd;
};

inline void GameHud::addContainer(hud_area                a_hud_area,
                                  Ogre::OverlayContainer* a_container)
{
  hud_overlays[a_hud_area]->add2D(a_container);
}

inline void GameHud::addElement(Ogre::SceneNode* a_scene_node)
{
  hud_overlay_3d->add3D(a_scene_node);
}

/** @brief clears the Ogre objects
 */
inline void destroyOverlayContainer(Ogre::OverlayContainer* a_overlay)
{
  Ogre::OverlayManager* manager = Ogre::OverlayManager::getSingletonPtr();
  assert(manager);

  // destroy child containers
  Ogre::OverlayContainer::ChildContainerIterator it2 = a_overlay->getChildContainerIterator();
  while (it2.hasMoreElements()) {
    Ogre::OverlayContainer* child_node = static_cast<Ogre::OverlayContainer*>(it2.getNext());
    destroyOverlayContainer(child_node);
  }

  // destroy child overlays
  Ogre::OverlayContainer::ChildIterator it = a_overlay->getChildIterator();
  while (it.hasMoreElements()) {
    Ogre::OverlayElement* overlay = static_cast<Ogre::OverlayElement*>(it.getNext());
    manager->destroyOverlayElement(overlay);
  }

  manager->destroyOverlayElement(a_overlay);
}

#endif // HUD_H
