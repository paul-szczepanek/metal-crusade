// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "input_handler.h"
#include "game.h"
#include "query_mask.h"
#include "game_camera.h"
#include "key_map.h"
#include "timer.h"
#include "game_controller.h"
#include "game_arena.h"

InputHandler::InputHandler()
  : sticky_left_shift(false), sticky_left_control(false)
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
  input_manager = OIS::InputManager::createInputSystem(param_list);

  // add keyboard to OIS
  keyboard = static_cast<OIS::Keyboard*>
             (input_manager->createInputObject(OIS::OISKeyboard, true));
  keyboard->setEventCallback(this);

  // add mouse to OIS
  mouse = static_cast<OIS::Mouse*>(input_manager->createInputObject(OIS::OISMouse, true));
  mouse->setEventCallback(this);

  // load keys from a config file into a key map
  key_map = new KeyMap();

  // handling resized windows
  Ogre::WindowEventUtilities::addWindowEventListener(Game::OgreWindow, this);

  // center pointer pos
  pointer_x = 0.5;
  pointer_y = 0.5;

  // ray query for finding where the pointer is in real world cords when using the mouse
  OgreCamera = Game::Camera->getOgreCamera();
  mouse_ray = new Ogre::Ray();
  mouse_ray_query = new Ogre::DefaultRaySceneQuery(Game::Scene);
  mouse_ray_query->setSortByDistance(true);
  mouse_ray_query->setQueryMask(query_mask_pointer_floor);

  // these two lines of redundancy allow to set up the camera and hud later on
  // as calling resize would also call the resize of camera nd hud and they may not exist yet
  screen_width = Game::OgreWindow->getWidth();
  screen_height = Game::OgreWindow->getHeight();

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
  pointer_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  pointer_node->attachObject(pointer_mesh); // attach mesh
  pointer_centre_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  pointer_centre_node->attachObject(pointer_centre_mesh); // attach mesh
}

InputHandler::~InputHandler()
{
  // yeah, this is missing bits but when this dies the whole program goes down anyway
  input_manager->destroyInputObject(mouse);
  input_manager->destroyInputObject(keyboard);
  OIS::InputManager::destroyInputSystem(input_manager);
}

/** @brief get pointer position in the world
 */
void InputHandler::updateMousePointer()
{
  // send a ray from camera to pointer position
  OgreCamera->getCameraToViewportRay(pointer_x, pointer_y, mouse_ray);

  // use the ray in the query
  mouse_ray_query->setRay(*mouse_ray);

  // execute query
  Ogre::RaySceneQueryResult &result = mouse_ray_query->execute();

  // iterate through results and find the nearest point
  Ogre::RaySceneQueryResult::iterator it;
  Real closestDistance = 10000; // search floor in case ray misses ecerything
  for(it = result.begin(); it != result.end(); ++it ) {
    if (closestDistance > (*it).distance) {
      closestDistance = (*it).distance;
    }
  }

  // clear query
  mouse_ray_query->clearResults();

  Vector3 position;

  if (closestDistance < 10000) {
    // pointer hit something! how exciting
    position = mouse_ray->getPoint(closestDistance);

  } else {
    // nothing in the way - fing the point where the ray crosses the terrain
    // find the the segment first because we have a rough idea where it is
    const Real step = 128;
    Real top_distance = CAMERA_DIST + step;
    Real bottom_distance = CAMERA_DIST - step;
    position = mouse_ray->getPoint(CAMERA_DIST);

    // check the position is within the arena to save on checking in the arena class
    // this means it just stretches the border height outside the map
    Game::Arena->isOutOfBounds(position); // TODO: deal with out of arena pointer

    // start at the distance the camera is from the focal point
    if (position.y < Game::Arena->getHeight(position.x, position.z)) {
      // search above camera point
      position = mouse_ray->getPoint(top_distance);
      Game::Arena->isOutOfBounds(position);
      while (position.y < Game::Arena->getHeight(position.x, position.z)) {
        top_distance -= step;
        position = mouse_ray->getPoint(top_distance);
        Game::Arena->isOutOfBounds(position); // inefficient fix, but what can you do
      }
      bottom_distance = top_distance - step;
    } else {
      // search below camera point
      position = mouse_ray->getPoint(bottom_distance);
      Game::Arena->isOutOfBounds(position);
      while (position.y > Game::Arena->getHeight(position.x, position.z)) {
        bottom_distance += step;
        position = mouse_ray->getPoint(bottom_distance);
        Game::Arena->isOutOfBounds(position); // inefficient fix
      }
      top_distance = bottom_distance + step;
    }

    // if this was university I'd be going all recursive on its arse,
    // but I need the overhead like I need to amuse you with these comments

    // half slice my way through
    Real middle_distance;
    Real slice_size = step;
    while (slice_size > 0.015625) {
      slice_size /= 2;
      middle_distance = top_distance + slice_size;
      position = mouse_ray->getPoint(middle_distance);
      Game::Arena->isOutOfBounds(position);
      if (position.y < Game::Arena->getHeight(position.x, position.z)) {
        bottom_distance = middle_distance;
      } else {
        top_distance = middle_distance;
      }
    }

    // why hello there little pointer
    position = mouse_ray->getPoint(middle_distance);
  }

  // pinter centre always at target high
  pointer_centre_node->setPosition(position
                                   + Vector3(0, target_high_offset, 0));

  // adjust by target mode
  if (game_controller->ControlBlock.target_high) {
    position.y += target_high_offset;
  } else if (game_controller->ControlBlock.target_air) {
    position.y += target_air_offset;
  } else {
    position.y += target_low_offset;
  }

  // pass the pointer pos to game cotnroller
  game_controller->setPointerPos(position);

  // move the outer target mesh
  pointer_node->setPosition(position);
}

/** @brief called every frame
 * @todo: add gamepad
 */
void InputHandler::capture()
{
  mouse->capture();
  keyboard->capture();
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
  game_controller = a_game_controller;
}

/** @brief called by Ogre on window resize
 */
void InputHandler::windowResized(Ogre::RenderWindow* render_window)
{
  screen_width = render_window->getWidth();
  screen_height = render_window->getHeight();

  // sanity check on window size
  if (screen_width < 320) { // min width 320
    screen_width = 320;
    render_window->resize(screen_width, screen_height);
  }
  if (screen_height < 240) { // min height 240
    screen_height = 240;
    render_window->resize(screen_width, screen_height);
  }

  // sanity check on aspect ratio
  if (screen_width < screen_height) { // limit portrait mode to square
    screen_height = screen_width;
    render_window->resize(screen_width, screen_height);
  } else if (screen_width > screen_height * 3) { // limit wide aspect ratio to 3:1
    screen_width = screen_height * 3;
    render_window->resize(screen_width, screen_height);
  }

  // change the window size for mouse input
  resize();

  // readjust the camera angle
  Game::Camera->resize(screen_width, screen_height);

  // rearragne the hud
  // Game::hud->resize(screen_width, screen_height);
}

/** @brief set mouse area
 */
void InputHandler::resize()
{
  const OIS::MouseState& mouse_state = mouse->getMouseState();
  mouse_state.width = screen_width;
  mouse_state.height = screen_height;
}

/** @brief updates mouse position inside the window
 */
bool InputHandler::mouseMoved(const OIS::MouseEvent& evt)
{
  // OIS mouse object
  const OIS::MouseState& mouse_state = mouse->getMouseState();

  // get mouse pos in pixels
  pointer_x = mouse_state.X.abs;
  pointer_y = mouse_state.Y.abs;
  // transalte to <0,1>
  pointer_x /= mouse_state.width;
  pointer_y /= mouse_state.height;

  return true;
}

bool InputHandler::mousePressed(const OIS::MouseEvent& evt,
                                OIS::MouseButtonID     btn)
{
  if (key_map->Buttons[input_event_fire] == btn) {
    game_controller->ControlBlock.fire = true;
    if (Game::instance()->getTimer()->getRate() == 0) {
      Game::instance()->getTimer()->unpause();
    }
  } else if (key_map->Buttons[input_event_turn_to_pointer] == btn) {
    game_controller->ControlBlock.turn_to_pointer = true;
  } else // if else chain continues below .v.

  // do once buttons
  if (key_map->Buttons[input_event_cycle_group] == btn) {
    game_controller->ControlBlock.cycle_group = true;
  } else if (key_map->Buttons[input_event_cycle_weapon] == btn) {
    game_controller->ControlBlock.cycle_weapon = true;
  } else if (key_map->Buttons[input_event_nearest_enemy] == btn) {
    game_controller->ControlBlock.nearest_enemy = true;
  } else if (key_map->Buttons[input_event_enemy] == btn) {
    game_controller->ControlBlock.enemy = true;
  } else if (key_map->Buttons[input_event_enemy_back] == btn) {
    game_controller->ControlBlock.enemy_back = true;
  } else if (key_map->Buttons[input_event_friendly] == btn) {
    game_controller->ControlBlock.friendly = true;
  } else if (key_map->Buttons[input_event_friendly_back] == btn) {
    game_controller->ControlBlock.friendly_back = true;
  } else if (key_map->Buttons[input_event_target_pointer] == btn) {
    game_controller->ControlBlock.target_pointer = true;
  } else if (key_map->Buttons[input_event_nav_point] == btn) {
    game_controller->ControlBlock.nav_point = true;
  } else if (key_map->Buttons[input_event_nav_point_back] == btn) {
    game_controller->ControlBlock.nav_point_back = true;
  } else if (key_map->Buttons[input_event_inspect] == btn) {
    game_controller->ControlBlock.inspect = true;
  } else if (key_map->Buttons[input_event_target_low] == btn) {
    game_controller->ControlBlock.target_low = true;
  } else if (key_map->Buttons[input_event_target_high] == btn) {
    game_controller->ControlBlock.target_high = true;
  } else if (key_map->Buttons[input_event_target_air] == btn) {
    game_controller->ControlBlock.target_air = true;
  } else if (key_map->Buttons[input_event_zoom_in] == btn) {
    game_controller->ControlBlock.zoom_in = true;
  } else if (key_map->Buttons[input_event_zoom_out] == btn) {
    game_controller->ControlBlock.zoom_out = true;
  } else if (key_map->Buttons[input_event_zoom_target] == btn) {
    game_controller->ControlBlock.zoom_target = true;
  } else if (key_map->Buttons[input_event_menu_interface] == btn) {
    game_controller->ControlBlock.menu_interface = true;
  } else if (key_map->Buttons[input_event_communication_interface] == btn) {
    game_controller->ControlBlock.communication_interface = true;
  }

  return true;
}

bool InputHandler::mouseReleased(const OIS::MouseEvent& evt,
                                 OIS::MouseButtonID     btn)
{
  if (key_map->Buttons[input_event_fire] == btn) {
    game_controller->ControlBlock.fire = false;
  }

  if (key_map->Buttons[input_event_turn_to_pointer] == btn) {
    game_controller->ControlBlock.turn_to_pointer = false;
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
    sticky_left_shift = true;
  } else if (OIS::KC_LCONTROL == evt.key) {
    sticky_left_shift = true;
  }

  // set throttle full speed in reverse unless moving forward, in that case stop
  if (key_map->Keys[input_event_down] == evt.key) {
    Real throttle = game_controller->getThrottle();
    if (throttle <= -0.5) {
      game_controller->setThrottle(-1);
    } else if (throttle <= 0) {
      game_controller->setThrottle(-0.5);
    } else if (throttle <= 0.25) {
      game_controller->setThrottle(0);
    } else if (throttle <= 0.5) {
      game_controller->setThrottle(0.25);
    } else if (throttle <= 0.75) {
      game_controller->setThrottle(0.5);
    } else {
      game_controller->setThrottle(0.75);
    }
  } else // if else chain continues below .v.

  // set throttle full speed ahead unless going backward, in that case stop
  if (key_map->Keys[input_event_up] == evt.key) {
    Real throttle = game_controller->getThrottle();
    if (throttle >= 0.75) {
      game_controller->setThrottle(1);
    } else if (throttle >= 0.5) {
      game_controller->setThrottle(0.75);
    } else if (throttle >= 0.25) {
      game_controller->setThrottle(0.5);
    } else if (throttle >= 0.0) {
      game_controller->setThrottle(0.25);
    } else if (throttle >= -0.5) {
      game_controller->setThrottle(0);
    } else {
      game_controller->setThrottle(-0.5);
    }
  } else   // if else chain continues below .v.

  // flip throttle into reverse (with the same value, zero is always positive)
  if (key_map->Keys[input_event_set_speed_reverse] == evt.key) {
    game_controller->setThrottle(-game_controller->getThrottle());
  } else     // if else chain continues below .v.
  // stop crusader
  if (key_map->Keys[input_event_set_speed_0] == evt.key) {
    game_controller->setThrottle(0);
  } else       // if else chain continues below .v.

  // setting throttle (checks to see if the crusader is in reverse)
  if (key_map->Keys[input_event_set_speed_1] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.1);
    } else {
      game_controller->setThrottle(-0.1);
    }
  } else if (key_map->Keys[input_event_set_speed_2] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.2);
    } else {
      game_controller->setThrottle(-0.2);
    }
  } else if (key_map->Keys[input_event_set_speed_3] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.3);
    } else {
      game_controller->setThrottle(-0.3);
    }
  } else if (key_map->Keys[input_event_set_speed_4] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.4);
    } else {
      game_controller->setThrottle(-0.4);
    }
  } else if (key_map->Keys[input_event_set_speed_5] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.5);
    } else {
      game_controller->setThrottle(-0.5);
    }
  } else if (key_map->Keys[input_event_set_speed_6] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.6);
    } else {
      game_controller->setThrottle(-0.6);
    }
  } else if (key_map->Keys[input_event_set_speed_7] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.7);
    } else {
      game_controller->setThrottle(-0.7);
    }
  } else if (key_map->Keys[input_event_set_speed_8] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.8);
    } else {
      game_controller->setThrottle(-0.8);
    }
  } else if (key_map->Keys[input_event_set_speed_9] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(0.9);
    } else {
      game_controller->setThrottle(-0.9);
    }
  } else if (key_map->Keys[input_event_set_speed_10] == evt.key) {
    if (game_controller->getThrottle() >= 0) {
      game_controller->setThrottle(1.0);
    } else {
      game_controller->setThrottle(-1.0);
    }
  } else         // if else chain continues below .v.

  // turn left and right
  if (key_map->Keys[input_event_left] == evt.key) {
    game_controller->setTurnSpeed(-1.0);
  } else if (key_map->Keys[input_event_right] == evt.key) {
    game_controller->setTurnSpeed(1.0);
  } else           // if else chain continues below .v.

  // toggles
  if (key_map->Keys[input_event_crouch] == evt.key) {
    game_controller->ControlBlock.crouch = (!game_controller->ControlBlock.crouch);
  } else if (key_map->Keys[input_event_fire_mode_group] == evt.key) {
    game_controller->ControlBlock.fire_mode_group =
      (!game_controller->ControlBlock.fire_mode_group);
  } else if (key_map->Keys[input_event_auto_cycle] == evt.key) {
    game_controller->ControlBlock.auto_cycle = (!game_controller->ControlBlock.auto_cycle);
  } else if (key_map->Keys[input_event_nightvision] == evt.key) {
    game_controller->ControlBlock.nightvision = (!game_controller->ControlBlock.nightvision);
  } else if (key_map->Keys[input_event_power] == evt.key) {
    game_controller->ControlBlock.power = (!game_controller->ControlBlock.power);
  } else if (key_map->Keys[input_event_radar] == evt.key) {
    game_controller->ControlBlock.radar = (!game_controller->ControlBlock.radar);
  } else if (key_map->Keys[input_event_lights] == evt.key) {
    game_controller->ControlBlock.lights = (!game_controller->ControlBlock.lights);
  } else             // if else chain continues below .v.

  // TEMP! until I decide how to handle input whilst in different modes
  // multitoggle focus for mfd buttons
  if (key_map->Keys[input_event_log] == evt.key) {
    if (game_controller->ControlBlock.log) {
      game_controller->ControlBlock.log = false;
    } else {
      game_controller->ControlBlock.log = true;
      game_controller->ControlBlock.menu_interface = false;
      game_controller->ControlBlock.communication_interface = false;
      game_controller->ControlBlock.mfd1_select = false;
      game_controller->ControlBlock.mfd2_select = false;
    }
  } else if (key_map->Keys[input_event_menu_interface] == evt.key) {
    if (game_controller->ControlBlock.menu_interface) {
      game_controller->ControlBlock.menu_interface = false;
    } else {
      game_controller->ControlBlock.log = false;
      game_controller->ControlBlock.menu_interface = true;
      game_controller->ControlBlock.communication_interface = false;
      game_controller->ControlBlock.mfd1_select = false;
      game_controller->ControlBlock.mfd2_select = false;
    }
  } else if (key_map->Keys[input_event_communication_interface] == evt.key) {
    if (game_controller->ControlBlock.communication_interface) {
      game_controller->ControlBlock.communication_interface = false;
    } else {
      game_controller->ControlBlock.log = false;
      game_controller->ControlBlock.menu_interface = false;
      game_controller->ControlBlock.communication_interface = true;
      game_controller->ControlBlock.mfd1_select = false;
      game_controller->ControlBlock.mfd2_select = false;
    }
  } else if (key_map->Keys[input_event_mfd1_select] == evt.key) {
    if (game_controller->ControlBlock.mfd1_select) {
      game_controller->ControlBlock.mfd1_select = false;
    } else {
      game_controller->ControlBlock.log = false;
      game_controller->ControlBlock.menu_interface = false;
      game_controller->ControlBlock.communication_interface = false;
      game_controller->ControlBlock.mfd1_select = true;
      game_controller->ControlBlock.mfd2_select = false;
    }
  } else if (key_map->Keys[input_event_mfd2_select] == evt.key) {
    if (game_controller->ControlBlock.mfd2_select) {
      game_controller->ControlBlock.mfd2_select = false;
    } else {
      game_controller->ControlBlock.log = false;
      game_controller->ControlBlock.menu_interface = false;
      game_controller->ControlBlock.communication_interface = false;
      game_controller->ControlBlock.mfd1_select = false;
      game_controller->ControlBlock.mfd2_select = true;
    }
  } else               // if else chain continues below .v.

  // three way toggle
  if (key_map->Keys[input_event_target_low] == evt.key) {
    game_controller->ControlBlock.target_low = true;
    game_controller->ControlBlock.target_high = false;
    game_controller->ControlBlock.target_air = false;
  } else if (key_map->Keys[input_event_target_high] == evt.key) {
    game_controller->ControlBlock.target_low = false;
    game_controller->ControlBlock.target_high = true;
    game_controller->ControlBlock.target_air = false;
  } else if (key_map->Keys[input_event_target_air] == evt.key) {
    game_controller->ControlBlock.target_low = false;
    game_controller->ControlBlock.target_high = false;
    game_controller->ControlBlock.target_air = true;
  } else                 // if else chain continues below .v.

  // modifiers
  if (key_map->Keys[input_event_flush_coolant] == evt.key) {
    game_controller->ControlBlock.flush_coolant = true;
  } else if (key_map->Keys[input_event_align_to_torso] == evt.key) {
    game_controller->ControlBlock.align_to_torso = true;
  } else if (key_map->Keys[input_event_fire_group_1] == evt.key) {
    game_controller->ControlBlock.fire_group_1 = true;
  } else if (key_map->Keys[input_event_fire_group_2] == evt.key) {
    game_controller->ControlBlock.fire_group_2 = true;
  } else if (key_map->Keys[input_event_fire_group_3] == evt.key) {
    game_controller->ControlBlock.fire_group_3 = true;
  } else if (key_map->Keys[input_event_fire_group_4] == evt.key) {
    game_controller->ControlBlock.fire_group_4 = true;
  } else if (key_map->Keys[input_event_fire_group_5] == evt.key) {
    game_controller->ControlBlock.fire_group_5 = true;
  } else if (key_map->Keys[input_event_fire_group_all] == evt.key) {
    game_controller->ControlBlock.fire_group_all = true;
  } else                   // if else chain continues below .v.

  // do once buttons
  if (key_map->Keys[input_event_nearest_enemy] == evt.key) {
    game_controller->ControlBlock.nearest_enemy = true;
  } else if (key_map->Keys[input_event_enemy] == evt.key) {
    game_controller->ControlBlock.enemy = true;
  } else if (key_map->Keys[input_event_enemy_back] == evt.key) {
    game_controller->ControlBlock.enemy_back = true;
  } else if (key_map->Keys[input_event_friendly] == evt.key) {
    game_controller->ControlBlock.friendly = true;
  } else if (key_map->Keys[input_event_friendly_back] == evt.key) {
    game_controller->ControlBlock.friendly_back = true;
  } else if (key_map->Keys[input_event_target_pointer] == evt.key) {
    game_controller->ControlBlock.target_pointer = true;
  } else if (key_map->Keys[input_event_nav_point] == evt.key) {
    game_controller->ControlBlock.nav_point = true;
  } else if (key_map->Keys[input_event_nav_point_back] == evt.key) {
    game_controller->ControlBlock.nav_point_back = true;
  } else if (key_map->Keys[input_event_inspect] == evt.key) {
    game_controller->ControlBlock.inspect = true;
  } else if (key_map->Keys[input_event_cycle_weapon] == evt.key) {
    game_controller->ControlBlock.cycle_weapon = true;
  } else if (key_map->Keys[input_event_cycle_group] == evt.key) {
    game_controller->ControlBlock.cycle_group = true;
  } else if (key_map->Keys[input_event_zoom_in] == evt.key) {
    game_controller->ControlBlock.zoom_in = true;
  } else if (key_map->Keys[input_event_zoom_out] == evt.key) {
    game_controller->ControlBlock.zoom_out = true;
  } else if (key_map->Keys[input_event_zoom_target] == evt.key) {
    game_controller->ControlBlock.zoom_target = true;
  } else if (key_map->Keys[input_event_radar_zoom_in] == evt.key) {
    game_controller->ControlBlock.radar_zoom_in = true;
  } else if (key_map->Keys[input_event_radar_zoom_out] == evt.key) {
    game_controller->ControlBlock.radar_zoom_out = true;
  } else if (key_map->Keys[input_event_mfd_up] == evt.key) {
    game_controller->ControlBlock.mfd_up = true;
  } else if (key_map->Keys[input_event_mfd_down] == evt.key) {
    game_controller->ControlBlock.mfd_down = true;
  } else if (key_map->Keys[input_event_mfd_left] == evt.key) {
    game_controller->ControlBlock.mfd_left = true;
  } else if (key_map->Keys[input_event_mfd_right] == evt.key) {
    game_controller->ControlBlock.mfd_right = true;
  } else if (key_map->Keys[input_event_mfd_toggle] == evt.key) {
    game_controller->ControlBlock.mfd_toggle = true;
  }

  return true;
}

bool InputHandler::keyReleased(const OIS::KeyEvent& evt)
{
  // sticky keys!
  if (OIS::KC_LSHIFT == evt.key) {
    sticky_left_shift = false;
  } else if (OIS::KC_LCONTROL == evt.key) {
    sticky_left_shift = false;
  }

  // stop turning unless already turning in the other direction
  if (key_map->Keys[input_event_left] == evt.key) {
    if (keyboard->isKeyDown(key_map->Keys[input_event_right])) {
      game_controller->setTurnSpeed(1.0);
    } else {
      game_controller->setTurnSpeed(0.0);
    }
  } else if (key_map->Keys[input_event_right] == evt.key) {
    if (keyboard->isKeyDown(key_map->Keys[input_event_left])) {
      game_controller->setTurnSpeed(-1.0);
    } else {
      game_controller->setTurnSpeed(0.0);
    }
  } else // if else chain continues below .v.

  // modifiers
  if (key_map->Keys[input_event_flush_coolant] == evt.key) {
    game_controller->ControlBlock.flush_coolant = false;
  } else if (key_map->Keys[input_event_align_to_torso] == evt.key) {
    game_controller->ControlBlock.align_to_torso = false;
  } else if (key_map->Keys[input_event_fire_group_1] == evt.key) {
    game_controller->ControlBlock.fire_group_1 = false;
  } else if (key_map->Keys[input_event_fire_group_2] == evt.key) {
    game_controller->ControlBlock.fire_group_2 = false;
  } else if (key_map->Keys[input_event_fire_group_3] == evt.key) {
    game_controller->ControlBlock.fire_group_3 = false;
  } else if (key_map->Keys[input_event_fire_group_4] == evt.key) {
    game_controller->ControlBlock.fire_group_4 = false;
  } else if (key_map->Keys[input_event_fire_group_5] == evt.key) {
    game_controller->ControlBlock.fire_group_5 = false;
  } else if (key_map->Keys[input_event_fire_group_all] == evt.key) {
    game_controller->ControlBlock.fire_group_all = false;
  } else   // if else chain continues below .v.

  // pausing
  if (key_map->Keys[input_event_pause] == evt.key) {
    if (Game::instance()->getTimer()->getRate() == 0) {
      Game::instance()->getTimer()->unpause();
    } else {
      Game::instance()->getTimer()->pause();
    }
  }

  // emergency kill game
  if ((OIS::KC_ESCAPE == evt.key) && !sticky_left_shift && !sticky_left_control) {
    Game::instance()->end();
  }

  return true;
}
