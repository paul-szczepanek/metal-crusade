//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

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

    OIS::KeyCode getKey(input_event a_evt) { return keys[a_evt]; };
    OIS::MouseButtonID getMouse(input_event a_evt) { return buttons[a_evt]; };

    //key config
    map<input_event, OIS::KeyCode> keys;
    map<input_event, OIS::MouseButtonID> buttons;
};

#endif // KEYMAP_H
