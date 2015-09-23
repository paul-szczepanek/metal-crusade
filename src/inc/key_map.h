// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef KEYMAP_H
#define KEYMAP_H

#include "main.h"
#include "input_event.h"

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

class KeyMap
{
public:
  KeyMap();
  ~KeyMap();

  OIS::KeyCode getKey(input_event a_evt) { return Keys[a_evt]; };
  OIS::MouseButtonID getMouse(input_event a_evt) { return Buttons[a_evt]; };

  bool getKeyConfig(const string& filename);
  OIS::MouseButtonID translateMouse(const string& id);
  OIS::KeyCode translateKeycode(const string& id);

  // key config
  map<input_event, OIS::KeyCode> Keys;
  map<input_event, OIS::MouseButtonID> Buttons;
};

#endif // KEYMAP_H
