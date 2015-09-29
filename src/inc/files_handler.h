// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef FILESHANDLER_H
#define FILESHANDLER_H

#include "main.h"
#include "crusader_design.h"

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

namespace FilesHandler
{
void stripWhitespace(string& a_string);
Real getReal(string a_string);
bool getBool(string a_string);
int getEnum(const string& a_string);
void getCharArray(vector<char>& char_array,
                  string        a_buffer);
void getStringArray(vector<string>& string_array,
                    string          a_buffer);
bool getPairs(const string&        filename,
              const string&        dir,
              map<string, string>& pairs);
string getStringFromFile(const string& filename);
void getRealSeries(vector<Real>& real_array,
                   string        a_buffer);
void getIntPairArray(vector<int_pair>& int_pair_array,
                     string            a_buffer);
void getColourArray(vector<Ogre::ColourValue>& colour_array,
                    string                     a_buffer);

void getSizeTArray(vector<size_t>& int_array,
                   string          a_buffer);
void getUsintArray(vector<usint>& int_array,
                   string         a_buffer);
void getIntArray(vector<int>& int_array,
                 string       a_buffer);
void getEnumArray(vector<long unsigned int>& enum_array,
                  string          a_buffer);
void getEnumArray(vector<crusader_part::body_part>& enum_array,
                  string          a_buffer);
void getEnumArray(vector<internal_type>& enum_array,
                  string          a_buffer);
void getUsintDigitArray(vector<usint>& int_array,
                        string         a_buffer);
void getIntDigitArray(vector<int>& int_array,
                      string       a_buffer);
void getSizeTDigitArray(vector<size_t>& int_array,
                        string          a_buffer);
}

#endif // FILESHANDLER_H
