// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "input_handler.h"
#include "game.h"
#include "query_mask.h"
#include "game_camera.h"
#include "key_map.h"
#include "game_timer.h"
#include "game_controller.h"
#include "game_arena.h"
#include "game_hud.h"

InputHandler::InputHandler()
  : StickyLeftShift(false), StickyLControl(false)
{
  // OIS configuration
  OIS::ParamList param_list;

  // get the window handle to hand it over to OIS
  unsigned long window_handle;
  Game::OgreWindow->getCustomAttribute("WINDOW", &window_handle);
  param_list.insert(OIS::ParamList::value_type("WINDOW",
                                               Ogre::StringConverter::toString(window_handle)));

  // to stop the mouse being trapped and keyboard madness
  param_list.insert(make_pair(string("x11_mouse_grab"), string("false")));
  param_list.insert(make_pair(string("x11_mouse_hide"), string("true")));
  param_list.insert(make_pair(string("x11_keyboard_grab"), string("false")));
  param_list.insert(make_pair(string("XAutoRepeatOn"), string("true")));

  // start OIS
  OISInputManager = OIS::InputManager::createInputSystem(param_list);

  // add keyboard to OIS
  OISKeyboard = static_cast<OIS::Keyboard*>
                (OISInputManager->createInputObject(OIS::OISKeyboard, true));
  OISKeyboard->setEventCallback(this);

  // add mouse to OIS
  OISMouse = static_cast<OIS::Mouse*>(OISInputManager->createInputObject(OIS::OISMouse, true));
  OISMouse->setEventCallback(this);

  // load keys from a config file into a key map
  Keys = new KeyMap();

  // handling resized windows
  Ogre::WindowEventUtilities::addWindowEventListener(Game::OgreWindow, this);

  // center pointer pos
  PointerX = 0.5;
  PointerY = 0.5;

  // ray query for finding where the pointer is in real world cords when using the mouse
  OgreCamera = Game::Camera->OgreCamera;
  MouseRay = new Ogre::Ray();
  MouseRayQuery = new Ogre::DefaultRaySceneQuery(Game::Scene);
  MouseRayQuery->setSortByDistance(true);
  MouseRayQuery->setQueryMask(query_mask_pointer_floor);

  // these two lines of redundancy allow to set up the camera and hud later on
  // as calling resize would also call the resize of camera nd hud and they may not exist yet
  ScreenW = Game::OgreWindow->getWidth();
  ScreenH = Game::OgreWindow->getHeight();

  // set the extents from where mouse input is taken
  resize();

  // create pointer mesh TODO: pointer mesh!
  Ogre::Entity* pointer_mesh = Game::Scene->createEntity("pointer", "target.mesh");
  Ogre::Entity* pointer_centre_mesh = Game::Scene->createEntity("pointer_centre",
                                                                "target_centre.mesh");
  pointer_mesh->setMaterialName("target");
  pointer_mesh->setQueryFlags(query_mask_ignore);
  pointer_mesh->setCastShadows(false);
  pointer_centre_mesh->setMaterialName("target");
  pointer_centre_mesh->setQueryFlags(query_mask_ignore);
  pointer_centre_mesh->setCastShadows(false);
  PointerNode = Game::Scene->getRootSceneNode()->createChildSceneNode();
  PointerNode->attachObject(pointer_mesh); // attach mesh
  PointerCentreNode = Game::Scene->getRootSceneNode()->createChildSceneNode();
  PointerCentreNode->attachObject(pointer_centre_mesh); // attach mesh
}

InputHandler::~InputHandler()
{
  // yeah, this is missing bits but when this dies the whole program goes down anyway
  OISInputManager->destroyInputObject(OISMouse);
  OISInputManager->destroyInputObject(OISKeyboard);
  OIS::InputManager::destroyInputSystem(OISInputManager);
}

Vector3 InputHandler::findTerrainCrossingPosition()
{
  // nothing in the way - find the point where the ray crosses the terrain
  // find the the segment first because we have a rough idea where it is
  const Real step = 128;
  const Real camera_distance = Game::Camera->CameraDistance;
  Real top_distance = camera_distance + step;
  Real bottom_distance = camera_distance - step;
  Vector3 position = MouseRay->getPoint(camera_distance);
  Vector3 ray_origin = MouseRay->getOrigin();
  Vector3 ray_direction = MouseRay->getDirection();

  //TODO: temp fix for broken camera
  if (ray_origin.y < CAMERA_CLIP_NEAR || ray_direction.y > -0.1) {
    // the ray is not pointing down towards the ground so we're not going to hit anything
    return Vector3::ZERO;
  }

  // check the position is within the arena to save on checking in the arena class
  // TODO: rewrite this mess
  Game::Arena->isOutOfBounds(position);

  // start at the distance the camera is from the focal point
  if (position.y < Game::Arena->getHeight(position.x, position.z)) {
    // search above camera point
    position = MouseRay->getPoint(top_distance);
    Game::Arena->isOutOfBounds(position);
    while (position.y < Game::Arena->getHeight(position.x, position.z)) {
      top_distance -= step;
      position = MouseRay->getPoint(top_distance);
      Game::Arena->isOutOfBounds(position); // inefficient fix, but what can you do
    }
    bottom_distance = top_distance - step;
  } else {
    // search below camera point
    position = MouseRay->getPoint(bottom_distance);
    Game::Arena->isOutOfBounds(position);
    while (position.y > Game::Arena->getHeight(position.x, position.z)) {
      bottom_distance += step;
      position = MouseRay->getPoint(bottom_distance);
      Game::Arena->isOutOfBounds(position); // inefficient fix
    }
    top_distance = bottom_distance + step;
  }

  // half slice my way through
  Real middle_distance;
  Real slice_size = step;
  while (slice_size > 0.015625) {
    slice_size /= 2;
    middle_distance = top_distance + slice_size;
    position = MouseRay->getPoint(middle_distance);
    Game::Arena->isOutOfBounds(position);
    if (position.y < Game::Arena->getHeight(position.x, position.z)) {
      bottom_distance = middle_distance;
    } else {
      top_distance = middle_distance;
    }
  }

  return MouseRay->getPoint(middle_distance);
}

/** @brief get pointer position in the world
 */
void InputHandler::updateMousePointer()
{
  // send a ray from camera to pointer position
  OgreCamera->getCameraToViewportRay(PointerX, PointerY, MouseRay);
  // use the ray in the query
  MouseRayQuery->setRay(*MouseRay);
  // execute query
  Ogre::RaySceneQueryResult &result = MouseRayQuery->execute();

  // iterate through results and find the nearest point
  Ogre::RaySceneQueryResult::iterator it;
  Real closest_distance = CAMERA_CLIP_FAR; // search floor in case ray misses everything
  for(it = result.begin(); it != result.end(); ++it ) {
    if (closest_distance > (*it).distance) {
      closest_distance = (*it).distance;
    }
  }

  // clear query
  MouseRayQuery->clearResults();

  Vector3 position;

  if (closest_distance < CAMERA_CLIP_FAR) {
    // pointer hit something! how exciting
    position = MouseRay->getPoint(closest_distance);
  } else {
    position = findTerrainCrossingPosition();
  }

  // pinter centre always at target high
  PointerCentreNode->setPosition(position + Vector3(0, target_high_offset, 0));

  // adjust by target mode
  if (Controller->ControlBlock.target_high) {
    position.y += target_high_offset;
  } else if (Controller->ControlBlock.target_air) {
    position.y += target_air_offset;
  } else {
    position.y += target_low_offset;
  }

  // move the outer target mesh
  PointerNode->setPosition(position);
  // pass the pointer pos to game cotnroller
  Controller->setPointerPos(position);
}

/** @brief called every frame
 * @todo: add gamepad
 */
void InputHandler::capture()
{
  OISMouse->capture();
  OISKeyboard->capture();
  updateMousePointer();
  // TODO: gamepad could be used as an alternative

  // TO DO: keep checking keys pressed until released to make sure we don't lose input
  // for example when window looses focus
  // if (keyboard->isKeyDown(OIS::KC_DOWN))
  // or deal with that in some other way
}

/** @brief binds a controller it should hand over the input to
 */
void InputHandler::bindController(GameController* a_game_controller)
{
  Controller = a_game_controller;
}

/** @brief called by Ogre on window resize
 */
void InputHandler::windowResized(Ogre::RenderWindow* render_window)
{
  ScreenW = render_window->getWidth();
  ScreenH = render_window->getHeight();

  // sanity check on window size
  if (ScreenW < 320) { // min width 320
    ScreenW = 320;
    render_window->resize(ScreenW, ScreenH);
  }
  if (ScreenH < 240) { // min height 240
    ScreenH = 240;
    render_window->resize(ScreenW, ScreenH);
  }

  // sanity check on aspect ratio
  if (ScreenW < ScreenH) { // limit portrait mode to square
    ScreenH = ScreenW;
    render_window->resize(ScreenW, ScreenH);
  } else if (ScreenW > ScreenH * 3) { // limit wide aspect ratio to 3:1
    ScreenW = ScreenH * 3;
    render_window->resize(ScreenW, ScreenH);
  }

  // change the window size for mouse input
  resize();

  // readjust the camera angle
  Game::Camera->resize(ScreenW, ScreenH);

  // rearragne the hud
  Game::Hud->resize(ScreenW, ScreenH);
}

/** @brief set mouse area
 */
void InputHandler::resize()
{
  const OIS::MouseState& mouse_state = OISMouse->getMouseState();
  mouse_state.width = ScreenW;
  mouse_state.height = ScreenH;
  Game::Camera->resize(ScreenW, ScreenH);
}

/** @brief updates mouse position inside the window
 */
bool InputHandler::mouseMoved(const OIS::MouseEvent& /*evt*/)
{
  // OIS mouse object
  const OIS::MouseState& mouse_state = OISMouse->getMouseState();

  // get mouse pos in pixels
  PointerX = mouse_state.X.abs;
  PointerY = mouse_state.Y.abs;
  // transalte to <0,1>
  PointerX /= mouse_state.width;
  PointerY /= mouse_state.height;

  return true;
}

bool InputHandler::mousePressed(const OIS::        MouseEvent& /*evt*/,
                                OIS::MouseButtonID btn)
{
  if (Keys->Buttons[input_event_fire] == btn) {
    Controller->ControlBlock.fire = true;
    if (Game::Timer->getRate() == 0) {
      Game::Timer->unpause();
    }
  } else if (Keys->Buttons[input_event_turn_to_pointer] == btn) {
    Controller->ControlBlock.turn_to_pointer = true;
  } else // if else chain continues below .v.

  // do once buttons
  if (Keys->Buttons[input_event_cycle_group] == btn) {
    Controller->ControlBlock.cycle_group = true;
  } else if (Keys->Buttons[input_event_cycle_weapon] == btn) {
    Controller->ControlBlock.cycle_weapon = true;
  } else if (Keys->Buttons[input_event_nearest_enemy] == btn) {
    Controller->ControlBlock.nearest_enemy = true;
  } else if (Keys->Buttons[input_event_enemy] == btn) {
    Controller->ControlBlock.enemy = true;
  } else if (Keys->Buttons[input_event_enemy_back] == btn) {
    Controller->ControlBlock.enemy_back = true;
  } else if (Keys->Buttons[input_event_friendly] == btn) {
    Controller->ControlBlock.friendly = true;
  } else if (Keys->Buttons[input_event_friendly_back] == btn) {
    Controller->ControlBlock.friendly_back = true;
  } else if (Keys->Buttons[input_event_target_pointer] == btn) {
    Controller->ControlBlock.target_pointer = true;
  } else if (Keys->Buttons[input_event_nav_point] == btn) {
    Controller->ControlBlock.nav_point = true;
  } else if (Keys->Buttons[input_event_nav_point_back] == btn) {
    Controller->ControlBlock.nav_point_back = true;
  } else if (Keys->Buttons[input_event_inspect] == btn) {
    Controller->ControlBlock.inspect = true;
  } else if (Keys->Buttons[input_event_target_low] == btn) {
    Controller->ControlBlock.target_low = true;
  } else if (Keys->Buttons[input_event_target_high] == btn) {
    Controller->ControlBlock.target_high = true;
  } else if (Keys->Buttons[input_event_target_air] == btn) {
    Controller->ControlBlock.target_air = true;
  } else if (Keys->Buttons[input_event_zoom_in] == btn) {
    Controller->ControlBlock.zoom_in = true;
  } else if (Keys->Buttons[input_event_zoom_out] == btn) {
    Controller->ControlBlock.zoom_out = true;
  } else if (Keys->Buttons[input_event_zoom_target] == btn) {
    Controller->ControlBlock.zoom_target = true;
  } else if (Keys->Buttons[input_event_menu_interface] == btn) {
    Controller->ControlBlock.menu_interface = true;
  } else if (Keys->Buttons[input_event_communication_interface] == btn) {
    Controller->ControlBlock.communication_interface = true;
  }

  return true;
}

bool InputHandler::mouseReleased(const OIS::        MouseEvent& /*evt*/,
                                 OIS::MouseButtonID btn)
{
  if (Keys->Buttons[input_event_fire] == btn) {
    Controller->ControlBlock.fire = false;
  }

  if (Keys->Buttons[input_event_turn_to_pointer] == btn) {
    Controller->ControlBlock.turn_to_pointer = false;
  }

  return true;
}

/** @brief transalte key presses into input events for the controller
 * this is going to be a nightmare to maintain
 * TODO: optimise this - put pointers to functions in a table indexed by input_event?
 */
bool InputHandler::keyPressed(const OIS::KeyEvent& evt)
{
  // sticky keys!
  if (OIS::KC_LSHIFT == evt.key) {
    StickyLeftShift = true;
  } else if (OIS::KC_LCONTROL == evt.key) {
    StickyLeftShift = true;
  }

  // set throttle full speed in reverse unless moving forward, in that case stop
  if (Keys->Keys[input_event_down] == evt.key) {
    Real throttle = Controller->getThrottle();
    if (throttle <= -0.5) {
      Controller->setThrottle(-1);
    } else if (throttle <= 0) {
      Controller->setThrottle(-0.5);
    } else if (throttle <= 0.25) {
      Controller->setThrottle(0);
    } else if (throttle <= 0.5) {
      Controller->setThrottle(0.25);
    } else if (throttle <= 0.75) {
      Controller->setThrottle(0.5);
    } else {
      Controller->setThrottle(0.75);
    }
  } else // if else chain continues below .v.

  // set throttle full speed ahead unless going backward, in that case stop
  if (Keys->Keys[input_event_up] == evt.key) {
    Real throttle = Controller->getThrottle();
    if (throttle >= 0.75) {
      Controller->setThrottle(1);
    } else if (throttle >= 0.5) {
      Controller->setThrottle(0.75);
    } else if (throttle >= 0.25) {
      Controller->setThrottle(0.5);
    } else if (throttle >= 0.0) {
      Controller->setThrottle(0.25);
    } else if (throttle >= -0.5) {
      Controller->setThrottle(0);
    } else {
      Controller->setThrottle(-0.5);
    }
  } else     // if else chain continues below .v.

  // flip throttle into reverse (with the same value, zero is always positive)
  if (Keys->Keys[input_event_set_speed_reverse] == evt.key) {
    Controller->setThrottle(-Controller->getThrottle());
  } else         // if else chain continues below .v.
  // stop crusader
  if (Keys->Keys[input_event_set_speed_0] == evt.key) {
    Controller->setThrottle(0);
  } else             // if else chain continues below .v.

  // setting throttle (checks to see if the crusader is in reverse)
  if (Keys->Keys[input_event_set_speed_1] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.1);
    } else {
      Controller->setThrottle(-0.1);
    }
  } else if (Keys->Keys[input_event_set_speed_2] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.2);
    } else {
      Controller->setThrottle(-0.2);
    }
  } else if (Keys->Keys[input_event_set_speed_3] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.3);
    } else {
      Controller->setThrottle(-0.3);
    }
  } else if (Keys->Keys[input_event_set_speed_4] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.4);
    } else {
      Controller->setThrottle(-0.4);
    }
  } else if (Keys->Keys[input_event_set_speed_5] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.5);
    } else {
      Controller->setThrottle(-0.5);
    }
  } else if (Keys->Keys[input_event_set_speed_6] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.6);
    } else {
      Controller->setThrottle(-0.6);
    }
  } else if (Keys->Keys[input_event_set_speed_7] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.7);
    } else {
      Controller->setThrottle(-0.7);
    }
  } else if (Keys->Keys[input_event_set_speed_8] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.8);
    } else {
      Controller->setThrottle(-0.8);
    }
  } else if (Keys->Keys[input_event_set_speed_9] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(0.9);
    } else {
      Controller->setThrottle(-0.9);
    }
  } else if (Keys->Keys[input_event_set_speed_10] == evt.key) {
    if (Controller->getThrottle() >= 0) {
      Controller->setThrottle(1.0);
    } else {
      Controller->setThrottle(-1.0);
    }
  } else                 // if else chain continues below .v.

  // turn left and right
  if (Keys->Keys[input_event_left] == evt.key) {
    Controller->setTurnSpeed(-1.0);
  } else if (Keys->Keys[input_event_right] == evt.key) {
    Controller->setTurnSpeed(1.0);
  } else                     // if else chain continues below .v.

  // toggles
  if (Keys->Keys[input_event_crouch] == evt.key) {
    Controller->ControlBlock.crouch = (!Controller->ControlBlock.crouch);
  } else if (Keys->Keys[input_event_fire_mode_group] == evt.key) {
    Controller->ControlBlock.fire_mode_group =
      (!Controller->ControlBlock.fire_mode_group);
  } else if (Keys->Keys[input_event_auto_cycle] == evt.key) {
    Controller->ControlBlock.auto_cycle = (!Controller->ControlBlock.auto_cycle);
  } else if (Keys->Keys[input_event_nightvision] == evt.key) {
    Controller->ControlBlock.nightvision = (!Controller->ControlBlock.nightvision);
  } else if (Keys->Keys[input_event_power] == evt.key) {
    Controller->ControlBlock.power = (!Controller->ControlBlock.power);
  } else if (Keys->Keys[input_event_radar] == evt.key) {
    Controller->ControlBlock.radar = (!Controller->ControlBlock.radar);
  } else if (Keys->Keys[input_event_lights] == evt.key) {
    Controller->ControlBlock.lights = (!Controller->ControlBlock.lights);
  } else                         // if else chain continues below .v.

  // TEMP! until I decide how to handle input whilst in different modes
  // multitoggle focus for mfd buttons
  if (Keys->Keys[input_event_log] == evt.key) {
    if (Controller->ControlBlock.log) {
      Controller->ControlBlock.log = false;
    } else {
      Controller->ControlBlock.log = true;
      Controller->ControlBlock.menu_interface = false;
      Controller->ControlBlock.communication_interface = false;
      Controller->ControlBlock.mfd1_select = false;
      Controller->ControlBlock.mfd2_select = false;
    }
  } else if (Keys->Keys[input_event_menu_interface] == evt.key) {
    if (Controller->ControlBlock.menu_interface) {
      Controller->ControlBlock.menu_interface = false;
    } else {
      Controller->ControlBlock.log = false;
      Controller->ControlBlock.menu_interface = true;
      Controller->ControlBlock.communication_interface = false;
      Controller->ControlBlock.mfd1_select = false;
      Controller->ControlBlock.mfd2_select = false;
    }
  } else if (Keys->Keys[input_event_communication_interface] == evt.key) {
    if (Controller->ControlBlock.communication_interface) {
      Controller->ControlBlock.communication_interface = false;
    } else {
      Controller->ControlBlock.log = false;
      Controller->ControlBlock.menu_interface = false;
      Controller->ControlBlock.communication_interface = true;
      Controller->ControlBlock.mfd1_select = false;
      Controller->ControlBlock.mfd2_select = false;
    }
  } else if (Keys->Keys[input_event_mfd1_select] == evt.key) {
    if (Controller->ControlBlock.mfd1_select) {
      Controller->ControlBlock.mfd1_select = false;
    } else {
      Controller->ControlBlock.log = false;
      Controller->ControlBlock.menu_interface = false;
      Controller->ControlBlock.communication_interface = false;
      Controller->ControlBlock.mfd1_select = true;
      Controller->ControlBlock.mfd2_select = false;
    }
  } else if (Keys->Keys[input_event_mfd2_select] == evt.key) {
    if (Controller->ControlBlock.mfd2_select) {
      Controller->ControlBlock.mfd2_select = false;
    } else {
      Controller->ControlBlock.log = false;
      Controller->ControlBlock.menu_interface = false;
      Controller->ControlBlock.communication_interface = false;
      Controller->ControlBlock.mfd1_select = false;
      Controller->ControlBlock.mfd2_select = true;
    }
  } else                             // if else chain continues below .v.

  // three way toggle
  if (Keys->Keys[input_event_target_low] == evt.key) {
    Controller->ControlBlock.target_low = true;
    Controller->ControlBlock.target_high = false;
    Controller->ControlBlock.target_air = false;
  } else if (Keys->Keys[input_event_target_high] == evt.key) {
    Controller->ControlBlock.target_low = false;
    Controller->ControlBlock.target_high = true;
    Controller->ControlBlock.target_air = false;
  } else if (Keys->Keys[input_event_target_air] == evt.key) {
    Controller->ControlBlock.target_low = false;
    Controller->ControlBlock.target_high = false;
    Controller->ControlBlock.target_air = true;
  } else                                 // if else chain continues below .v.

  // modifiers
  if (Keys->Keys[input_event_flush_coolant] == evt.key) {
    Controller->ControlBlock.flush_coolant = true;
  } else if (Keys->Keys[input_event_align_to_torso] == evt.key) {
    Controller->ControlBlock.align_to_torso = true;
  } else if (Keys->Keys[input_event_fire_group_1] == evt.key) {
    Controller->ControlBlock.fire_group_1 = true;
  } else if (Keys->Keys[input_event_fire_group_2] == evt.key) {
    Controller->ControlBlock.fire_group_2 = true;
  } else if (Keys->Keys[input_event_fire_group_3] == evt.key) {
    Controller->ControlBlock.fire_group_3 = true;
  } else if (Keys->Keys[input_event_fire_group_4] == evt.key) {
    Controller->ControlBlock.fire_group_4 = true;
  } else if (Keys->Keys[input_event_fire_group_5] == evt.key) {
    Controller->ControlBlock.fire_group_5 = true;
  } else if (Keys->Keys[input_event_fire_group_all] == evt.key) {
    Controller->ControlBlock.fire_group_all = true;
  } else                                     // if else chain continues below .v.

  // do once buttons
  if (Keys->Keys[input_event_nearest_enemy] == evt.key) {
    Controller->ControlBlock.nearest_enemy = true;
  } else if (Keys->Keys[input_event_enemy] == evt.key) {
    Controller->ControlBlock.enemy = true;
  } else if (Keys->Keys[input_event_enemy_back] == evt.key) {
    Controller->ControlBlock.enemy_back = true;
  } else if (Keys->Keys[input_event_friendly] == evt.key) {
    Controller->ControlBlock.friendly = true;
  } else if (Keys->Keys[input_event_friendly_back] == evt.key) {
    Controller->ControlBlock.friendly_back = true;
  } else if (Keys->Keys[input_event_target_pointer] == evt.key) {
    Controller->ControlBlock.target_pointer = true;
  } else if (Keys->Keys[input_event_nav_point] == evt.key) {
    Controller->ControlBlock.nav_point = true;
  } else if (Keys->Keys[input_event_nav_point_back] == evt.key) {
    Controller->ControlBlock.nav_point_back = true;
  } else if (Keys->Keys[input_event_inspect] == evt.key) {
    Controller->ControlBlock.inspect = true;
  } else if (Keys->Keys[input_event_cycle_weapon] == evt.key) {
    Controller->ControlBlock.cycle_weapon = true;
  } else if (Keys->Keys[input_event_cycle_group] == evt.key) {
    Controller->ControlBlock.cycle_group = true;
  } else if (Keys->Keys[input_event_zoom_in] == evt.key) {
    Controller->ControlBlock.zoom_in = true;
  } else if (Keys->Keys[input_event_zoom_out] == evt.key) {
    Controller->ControlBlock.zoom_out = true;
  } else if (Keys->Keys[input_event_zoom_target] == evt.key) {
    Controller->ControlBlock.zoom_target = true;
  } else if (Keys->Keys[input_event_radar_zoom_in] == evt.key) {
    Controller->ControlBlock.radar_zoom_in = true;
  } else if (Keys->Keys[input_event_radar_zoom_out] == evt.key) {
    Controller->ControlBlock.radar_zoom_out = true;
  } else if (Keys->Keys[input_event_mfd_up] == evt.key) {
    Controller->ControlBlock.mfd_up = true;
  } else if (Keys->Keys[input_event_mfd_down] == evt.key) {
    Controller->ControlBlock.mfd_down = true;
  } else if (Keys->Keys[input_event_mfd_left] == evt.key) {
    Controller->ControlBlock.mfd_left = true;
  } else if (Keys->Keys[input_event_mfd_right] == evt.key) {
    Controller->ControlBlock.mfd_right = true;
  } else if (Keys->Keys[input_event_mfd_toggle] == evt.key) {
    Controller->ControlBlock.mfd_toggle = true;
  }

  return true;
}

bool InputHandler::keyReleased(const OIS::KeyEvent& evt)
{
  // sticky keys!
  if (OIS::KC_LSHIFT == evt.key) {
    StickyLeftShift = false;
  } else if (OIS::KC_LCONTROL == evt.key) {
    StickyLeftShift = false;
  }

  // stop turning unless already turning in the other direction
  if (Keys->Keys[input_event_left] == evt.key) {
    if (OISKeyboard->isKeyDown(Keys->Keys[input_event_right])) {
      Controller->setTurnSpeed(1.0);
    } else {
      Controller->setTurnSpeed(0.0);
    }
  } else if (Keys->Keys[input_event_right] == evt.key) {
    if (OISKeyboard->isKeyDown(Keys->Keys[input_event_left])) {
      Controller->setTurnSpeed(-1.0);
    } else {
      Controller->setTurnSpeed(0.0);
    }
  } else // if else chain continues below .v.

  // modifiers
  if (Keys->Keys[input_event_flush_coolant] == evt.key) {
    Controller->ControlBlock.flush_coolant = false;
  } else if (Keys->Keys[input_event_align_to_torso] == evt.key) {
    Controller->ControlBlock.align_to_torso = false;
  } else if (Keys->Keys[input_event_fire_group_1] == evt.key) {
    Controller->ControlBlock.fire_group_1 = false;
  } else if (Keys->Keys[input_event_fire_group_2] == evt.key) {
    Controller->ControlBlock.fire_group_2 = false;
  } else if (Keys->Keys[input_event_fire_group_3] == evt.key) {
    Controller->ControlBlock.fire_group_3 = false;
  } else if (Keys->Keys[input_event_fire_group_4] == evt.key) {
    Controller->ControlBlock.fire_group_4 = false;
  } else if (Keys->Keys[input_event_fire_group_5] == evt.key) {
    Controller->ControlBlock.fire_group_5 = false;
  } else if (Keys->Keys[input_event_fire_group_all] == evt.key) {
    Controller->ControlBlock.fire_group_all = false;
  } else     // if else chain continues below .v.

  // pausing
  if (Keys->Keys[input_event_pause] == evt.key) {
    if (Game::Timer->getRate() == 0) {
      Game::Timer->unpause();
    } else {
      Game::Timer->pause();
    }
  }

  // emergency kill game
  if ((OIS::KC_ESCAPE == evt.key) && !StickyLeftShift && !StickyLControl) {
    Game::instance()->end();
  }

  return true;
}
