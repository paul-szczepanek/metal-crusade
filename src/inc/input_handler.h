// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef OIS_INPUT_H
#define OIS_INPUT_H

class KeyMap;
class GameController;

#include "main.h"

#ifdef PLATFORM_WIN32
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISMouse.h>
#include <OISKeyboard.h>
#include <OISJoyStick.h>
#else
#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISJoyStick.h>
#endif

#include "input_event.h"

class InputHandler
  : public OIS::KeyListener,
  public OIS::MouseListener,
  public Ogre::WindowEventListener
{
public:
  InputHandler();
  ~InputHandler();

  void capture();

  // called by Ogre whe the window is resized
  void windowResized(Ogre::RenderWindow* render_window);
  // sets the mouse screen size
  void resize();

  // called by OIS on input
  bool mouseMoved(const OIS::MouseEvent& evt);
  bool mousePressed(const OIS::MouseEvent& evt,
                    OIS::                  MouseButtonID);
  bool mouseReleased(const OIS::MouseEvent& evt,
                     OIS::                  MouseButtonID);
  bool keyPressed(const OIS::KeyEvent& evt);
  bool keyReleased(const OIS::KeyEvent& evt);

  // bind the controller which receives local input
  void bindController(GameController* a_game_controller);

  // translate mouse position to 3D coords in the world
  void updateMousePointer();

private:
  Vector3 findTerrainCrossingPosition();

private:
  OIS::InputManager* OISInputManager;
  OIS::Mouse* OISMouse;
  OIS::Keyboard* OISKeyboard;

  // keyconfig
  KeyMap* Keys;

  // sticky keys!
  bool StickyLeftShift;
  bool StickyLControl;

  // local controller
  GameController* Controller;

  // mouse translation
  Ogre::Camera* OgreCamera;
  Ogre::RaySceneQuery* MouseRayQuery;
  Ogre::Ray* MouseRay;
  Real PointerX, PointerY;

  Ogre::SceneNode* PointerNode;
  Ogre::SceneNode* PointerCentreNode;

  // screen size
  size_t ScreenW;
  size_t ScreenH;
};

#endif // OIS_INPUT_H
