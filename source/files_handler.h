//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef FILESHANDLER_H
#define FILESHANDLER_H

#include "main.h"
#include "input_event.h"
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>

struct weapon_spec_t;
struct crusader_design_t;
struct crusader_engine_t;
struct crusader_drive_t;
struct crusader_chasis_t;
struct crusader_model_t;
struct hud_design_t;

class FilesHandler
{
public:
    ~FilesHandler() { };
    //singleton
    static void instantiate();
    static FilesHandler* instance();

    //read key config from file
    static bool getKeyConfig(const string& filename, map<input_event, OIS::KeyCode>& key_map,
                             map<input_event, OIS::MouseButtonID>& mouse_map);

    //read crusader design from file
    static bool getCrusaderDesign(const string& filename, crusader_design_t& design,
                                  crusader_engine_t& engine, crusader_drive_t& drive,
                                  crusader_chasis_t& chasis, crusader_model_t& model);
    static bool getCrusaderSpec(const string& filename, crusader_engine_t& engine,
                                crusader_drive_t& drive, crusader_chasis_t& chasis,
                                crusader_model_t& model);
    static bool getWeaponSpec(const string& filename, weapon_spec_t& weapon_spec);

    //read weapon design from file
    static bool getHudDesign(hud_design_t& hud_design, const string& filename);

    //game strings
    static bool getGameText(vector<string>& game_text);
    static ulint getStringKey(const string& a_string);

private:
    FilesHandler() { };
    //singleton
    static FilesHandler* inst;

    //OIS keycode translation
    static OIS::KeyCode translateKeycode(const string& id);
    static OIS::MouseButtonID translateMouse(const string& id);

    //sanitation for strings
    static void stripWhitespace(string& a_string);

    //helper funcitons for processing all the strings read from files into appropriate types
    static Ogre::Real getReal(string a_string);
    static int getEnum(const string& a_string);
    static bool getPairs(const string& filename, const string& dir, map<string, string>& pairs);
    static void getStringArray(vector<string>& string_array, string a_buffer);
    template <typename T> static void getIntArray(vector<T>& int_array, string a_buffer);
    template <typename T> static void getEnumArray(vector<T>& enum_array, string a_buffer);
    static void getIntPairArray(vector<int_pair>& int_pair_array, string a_buffer);
    template <class T1, class T2>
    static void getEnumPairArray(vector<pair<T1, T2> >& position_pair_array, string a_buffer);
    static void getRealSeries(vector<Ogre::Real>& real_array, string a_buffer);
    static void getColourArray(vector<Ogre::ColourValue>& colour_array, string a_buffer);

    //repeats the last colour to pad out the missing colours
    static void padHudColours(vector<Ogre::ColourValue>& colour_array);

    //stores game string keys used in files
    static map<string, ulint> string_key_map;
};

#endif // FILESHANDLER_H
