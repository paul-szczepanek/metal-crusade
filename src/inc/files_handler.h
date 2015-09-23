// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FILESHANDLER_H
#define FILESHANDLER_H

#include "main.h"
#include "input_event.h"

#ifdef PLATFORM_WIN32
#include <OISMouse.h>
#include <OISKeyboard.h>
#else
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>
#endif

class FilesHandler
{
public:
  static void init();
  ~FilesHandler() {
  }

  // read file as string (still parses for comments)
  static string getStringFromFile(const string& filename);

  // helper funcitons for processing all the strings read from files into appropriate types
  static Real getReal(string a_string);
  static bool getBool(string a_string);
  static int getEnum(const string& a_string);
  static bool getPairs(const string& filename,
                       const string& dir,
                       map<string, string>& pairs);
  static void getStringArray(vector<string>& string_array,
                             string          a_buffer);
  template <typename T> static void getIntArray(vector<T>& int_array,
                                                string     a_buffer);
  template <typename T> static void getEnumArray(vector<T>& enum_array,
                                                 string     a_buffer);
  static void getIntPairArray(vector<int_pair>& int_pair_array,
                              string            a_buffer);
  template <class T1, class T2>
  static void getEnumPairArray(vector<pair<T1, T2> >& position_pair_array,
                               string a_buffer);
  static void getRealSeries(vector<Real>& real_array,
                            string        a_buffer);
  static void getColourArray(vector<Ogre::ColourValue>& colour_array,
                             string                     a_buffer);
  static void getCharArray(vector<char>& char_array,
                           string        a_buffer);
  template <typename T> static void getDigitArray(vector<T>& int_array,
                                                  string     a_buffer);

  // sanitation for strings
  static void stripWhitespace(string& a_string);

private:
  FilesHandler() {
  }

};

/** @brief fills the vector with int but treats each character as an int so 82 is 8 and 2
 */
template <typename T> void FilesHandler::getDigitArray(vector<T>& int_array,
                                                       string     a_buffer)
{
  for(int i = 0, for_size = a_buffer.size(); i < for_size; ++i) {
    // this will fail for ";" so check for them since this might be multiline buffer
    if(a_buffer[i] != ';') {
      int integer = static_cast<T>(intoInt(a_buffer[i]));
      int_array.push_back(integer);
    }
  }
}

typedef FilesHandler FS;

const string DATA_DIR = "data/";
const string CONFIG_DIR = "config/";
const string WEAPON_DIR = "design/weapon/";
const string CRUSADER_DIR = "design/crusader/";
const string HUD_DIR = "design/hud/";
const string RADAR_DIR = "design/radar/";
const string UNIT_DIR = "unit/";
const string TEXT_DIR = "text/";
const string MODEL_DIR = "model/";
const string TERRAIN_DIR = "terrain/";
const string EXT_TERRAIN = ".terrain";

#endif // FILESHANDLER_H
