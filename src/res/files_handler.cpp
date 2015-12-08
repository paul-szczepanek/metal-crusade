// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "files_handler.h"
#include "game.h"
#include "hud_design.h"

namespace FilesHandler
{
void getSizeTArray(vector<size_t>& int_array,
                   string          a_buffer)
{
  getIntArrayT(int_array, a_buffer);
}

void getUsintArray(vector<usint>& int_array,
                   string         a_buffer)
{
  getIntArrayT(int_array, a_buffer);
}

void getIntArray(vector<int>& int_array,
                 string       a_buffer)
{
  getIntArrayT(int_array, a_buffer);
}

void getEnumArray(vector<crusader_part::body_part>& enum_array,
                  string                            a_buffer)
{
  getEnumArrayT(enum_array, a_buffer);
}

void getEnumArray(vector<internal_type>& enum_array,
                  string                 a_buffer)
{
  getEnumArrayT(enum_array, a_buffer);
}

void getUsintDigitArray(vector<usint>& int_array,
                        string         a_buffer)
{
  getDigitArrayT(int_array, a_buffer);
}

void getIntDigitArray(vector<int>& int_array,
                      string       a_buffer)
{
  getDigitArrayT(int_array, a_buffer);
}

void getSizeTDigitArray(vector<size_t>& int_array,
                        string          a_buffer)
{
  getDigitArrayT(int_array, a_buffer);
}

/** @brief strips leading and trailing spaces
 */
void stripWhitespace(string& a_string)
{
  if(!a_string.empty()) {
    size_t begin = a_string.find_first_not_of(" ");
    size_t end = a_string.find_last_not_of(" ");
    a_string = a_string.substr(begin, end - begin + 1);
  }
}

/** @brief string to float with additional sanitation
 */
Real getReal(string a_string)
{
  // basic sanitation, strip non numbers
  for(size_t b = 0, for_size = a_string.size(); b < for_size; ++b) {
    if(!isdigit(a_string.at(b)) && !ispunct(a_string.at(b))) {
      a_string.erase(b, 1);
    }
  }

  return intoReal(a_string);
}

/** @brief true/false string to bool
 */
bool getBool(string a_string)
{
  return (a_string == "true");
}

/** @brief string to int later to be cast into an enum,
 * I know it's a bit ugly if it gets too long it can be seperated into seperate enums
 * TODO: it's getting long, if you start using it outside of files ffs split it up
 */
int getEnum(const string& a_string)
{
  int enum_int = -1;

  if(a_string == "drive_type_limbs") {  // drive_type
    enum_int = 0;
  } else if(a_string == "drive_type_wheels") {
    enum_int = 1;
  } else if(a_string == "drive_type_tracks") {
    enum_int = 2;
  } else if(a_string == "drive_type_float") {
    enum_int = 3;
  } else if(a_string == "drive_type_hover") {
    enum_int = 4;
  } else if(a_string == "drive_subtype_biped") {  // drive_subtype
    enum_int = 0;
  } else if(a_string == "drive_subtype_quadruped") {
    enum_int = 1;
  } else if(a_string == "drive_subtype_multiped") {
    enum_int = 2;
  } else if(a_string == "drive_subtype_bicycle") {
    enum_int = 3;
  } else if(a_string == "drive_subtype_tricycle") {
    enum_int = 4;
  } else if(a_string == "drive_subtype_quad") {
    enum_int = 5;
  } else if(a_string == "drive_subtype_multi_wheel") {
    enum_int = 6;
  } else if(a_string == "drive_subtype_unitrack") {
    enum_int = 7;
  } else if(a_string == "drive_subtype_tracks") {
    enum_int = 8;
  } else if(a_string == "drive_subtype_multitracks") {
    enum_int = 9;
  } else if(a_string == "drive_subtype_hovercraft") {
    enum_int = 10;
  } else if(a_string == "drive_subtype_boat") {
    enum_int = 11;
  } else if(a_string == "drive_subtype_rocket") {
    enum_int = 12;
  } else if(a_string == "crusader_type_recon") {  // crusader_type
    enum_int = 0;
  } else if(a_string == "crusader_type_infantry") {
    enum_int = 1;
  } else if(a_string == "crusader_type_assault") {
    enum_int = 2;
  } else if(a_string == "crusader_type_goliath") {
    enum_int = 3;
  } else if(a_string == "crusader_type_cavarly") {
    enum_int = 4;
  } else if(a_string == "crusader_type_messanger") {
    enum_int = 5;
  } else if(a_string == "weapon_type_cannon") {  // weapon_type
    enum_int = 0;
  } else if(a_string == "weapon_type_launcher_missile") {
    enum_int = 1;
  } else if(a_string == "weapon_type_energy") {
    enum_int = 2;
  } else if(a_string == "weapon_type_launcher") {
    enum_int = 3;
  } else if(a_string == "armour_type_composite") {  // armour_type
    enum_int = 0;
  } else if(a_string == "armour_type_reactive") {
    enum_int = 1;
  } else if(a_string == "armour_type_plastic") {
    enum_int = 2;
  } else if(a_string == "armour_type_ceramic") {
    enum_int = 3;
  } else if(a_string == "armour_type_tesla") {
    enum_int = 4;
  } else if(a_string == "internal_type_struts") {  // internal_type
    enum_int = 0;
  } else if(a_string == "internal_type_structure") {
    enum_int = 1;
  } else if(a_string == "internal_type_heatsink") {
    enum_int = 2;
  } else if(a_string == "internal_type_actuator") {
    enum_int = 3;
  } else if(a_string == "internal_type_gyro") {
    enum_int = 4;
  } else if(a_string == "internal_type_coolant") {
    enum_int = 5;
  } else if(a_string == "internal_type_ultra_gyro") {
    enum_int = 6;
  } else if(a_string == "internal_type_capacitor") {
    enum_int = 7;
  } else if(a_string == "torso") {  // body part
    enum_int = 0;
  } else if(a_string == "torso_right") {
    enum_int = 1;
  } else if(a_string == "torso_left") {
    enum_int = 2;
  } else if(a_string == "arm_right") {
    enum_int = 3;
  } else if(a_string == "arm_left") {
    enum_int = 4;
  } else if(a_string == "leg_right") {
    enum_int = 5;
  } else if(a_string == "leg_left") {
    enum_int = 6;
  } else if(a_string == "leg_right_back") {
    enum_int = 7;
  } else if(a_string == "leg_left_back") {
    enum_int = 8;
  } else if(a_string == "left") {  // horizontal::position
    enum_int = 0;
  } else if(a_string == "centre") {
    enum_int = 1;
  } else if(a_string == "right") {
    enum_int = 2;
  } else if(a_string == "top") {  // vertical::position
    enum_int = 0;
  }
  // else if (a_string == "centre") already handled above
  //    enum_int = 1;
  else if(a_string == "bottom") {
    enum_int = 2;
  } else if(a_string == "hud_mfd1") {  // hud area
    enum_int = 0;
  } else if(a_string == "hud_mfd2") {
    enum_int = 1;
  } else if(a_string == "hud_radar") {
    enum_int = 2;
  } else if(a_string == "hud_weapon") {
    enum_int = 3;
  } else if(a_string == "hud_heat") {
    enum_int = 4;
  } else if(a_string == "hud_speed") {
    enum_int = 5;
  } else if(a_string == "hud_status") {
    enum_int = 6;
  } else if(a_string == "hud_log") {
    enum_int = 7;
  } else if(a_string == "mfd") {  // hud part function
    enum_int = 0;
  } else if(a_string == "mfd_aux") {
    enum_int = 1;
  } else if(a_string == "radar") {
    enum_int = 2;
  } else if(a_string == "radar_power") {
    enum_int = 3;
  } else if(a_string == "clock_tick") {
    enum_int = 4;
  } else if(a_string == "direction") {
    enum_int = 5;
  } else if(a_string == "torso_direction") {
    enum_int = 6;
  } else if(a_string == "compass") {
    enum_int = 7;
  } else if(a_string == "clock") {
    enum_int = 8;
  } else if(a_string == "radar_range") {
    enum_int = 9;
  } else if(a_string == "weapon_group") {
    enum_int = 10;
  } else if(a_string == "weapon_target") {
    enum_int = 11;
  } else if(a_string == "weapon_auto") {
    enum_int = 12;
  } else if(a_string == "weapons") {
    enum_int = 13;
  } else if(a_string == "log") {
    enum_int = 14;
  } else if(a_string == "temperature") {
    enum_int = 15;
  } else if(a_string == "temperature_external") {
    enum_int = 16;
  } else if(a_string == "coolant") {
    enum_int = 17;
  } else if(a_string == "speed") {
    enum_int = 18;
  } else if(a_string == "throttle") {
    enum_int = 19;
  } else if(a_string == "pressure") {
    enum_int = 20;
  } else if(a_string == "temperature_engine") {
    enum_int = 21;
  } else if(a_string == "status") {
    enum_int = 22;
  } else if(a_string == "mfd_military") {  // hud part type
    enum_int = 0;
  } else if(a_string == "mfd_aux_military") {
    enum_int = 1;
  } else if(a_string == "radar_military") {
    enum_int = 2;
  } else if(a_string == "set") {
    enum_int = 3;
  } else if(a_string == "pointer") {
    enum_int = 4;
  } else if(a_string == "text") {
    enum_int = 5;
  } else if(a_string == "weapon_tab") {
    enum_int = 6;
  } else if(a_string == "log_printer") {
    enum_int = 7;
  } else if(a_string == "dial") {
    enum_int = 8;
  } else if(a_string == "status_display") {
    enum_int = 9;
  } else if(a_string == "damage_diagram_self") {  // mfd view types
    enum_int = 0;
  } else if(a_string == "damage_diagram_target") {
    enum_int = 1;
  } else if(a_string == "damage_view_target") {
    enum_int = 2;
  } else if(a_string == "inspector") {
    enum_int = 3;
  } else if(a_string == "minimap") {
    enum_int = 4;
  } else if(a_string == "enemy_list") {
    enum_int = 5;
  } else if(a_string == "systems_control") {
    enum_int = 6;
  } else if(a_string == "squad_tactic") {
    enum_int = 7;
  } else if(a_string == "rotating") {  // radar
    enum_int = 0;
  } else if(a_string == "fixed") {
    enum_int = 1;
  } else if(a_string == "oscilating") {
    enum_int = 2;
  } else { // if all fails print for debug
    cout << "malformed enum: " << a_string << endl;
  }

  assert(enum_int > -1);

  return enum_int;
}

/** @brief fills the vector with chars from an array, basically ditches the \0, oh the idiocy
 * this is planned to do some sanitasation later on
 */
void getCharArray(vector<char>& char_array,
                  string        a_buffer)
{
  for(int i = 0, for_size = a_buffer.size(); i < for_size; ++i) {
    char_array.push_back(a_buffer[i]);
  }
}

/** @brief fills structs with strings read from a file
 */
void getStringArray(vector<string>& string_array,
                    string          a_buffer)
{
  size_t end = 0;
  string value;

  while(!a_buffer.empty()) {
    // semicolon seperated values
    end = a_buffer.find_first_of(";");

    // if there's a ';'
    if(end < a_buffer.size()) {
      // take one value
      value = a_buffer.substr(0, end);

      // cut it from the buffer
      a_buffer = a_buffer.substr(end + 1);

    } else {
      // take the whole buffer as the lasttile_types value
      value = a_buffer;
      a_buffer.clear();
    }

    stripWhitespace(value);  // skip leading and trailing whitespace

    // put values in the array as you find them
    if(!value.empty()) {
      string_array.push_back(value);
    }
  }
}

/** @brief fills structs with ints read from a file and casts them to enum
 */
template <typename T> void getEnumArrayT(vector<T>& enum_array,
                                         string     a_buffer)
{
  vector<string> string_array;

  // get strings into the vector
  getStringArray(string_array, a_buffer);

  // and turn them into enums
  for(size_t i = 0, for_size = string_array.size(); i < for_size; ++i) {
    enum_array.push_back(static_cast<T>(getEnum(string_array[i])));
  }
}

/** @brief fills structs with ints read from a file
 */
template <typename T> void getIntArrayT(vector<T>& int_array,
                                        string     a_buffer)
{
  size_t end = 0;
  string value;
  stripWhitespace(a_buffer);
  while(!a_buffer.empty()) {
    end = a_buffer.find_first_of(";");  // semicolon seperated values

    // if there's a ';'
    if(end < a_buffer.size()) {
      // take one value
      value = a_buffer.substr(0, end);

      // cut it from the buffer
      a_buffer = a_buffer.substr(end + 1);

    } else {
      // take the whole buffer as the last value
      value = a_buffer;
      a_buffer.clear();
    }

    // insert values as you find them
    if(!value.empty()) {
      int_array.push_back(intoInt(value));
    }
  }
}

/** @brief get pairs of strings form a file
 * it's the workhorse of parsing files into a map for most functions - parses file of a format:
 * [key] #comments
 * value #comments
 * and returns pairs of strings
 */
bool getPairs(const string& filename,
              const string& dir,
              map<string, string>& pairs)
{
  string buffer;
  string key; // [key] ignore whatver is after ]
  string value; // whatever is on next line except #comments
  string end_value; // multiline result
  string fs_filename = DATA_DIR + dir + filename;
  ifstream spec_file;

  spec_file.open(fs_filename.c_str());

  if(spec_file.is_open()) {
    size_t begin;
    size_t end;

    while(!spec_file.eof()) {
      getline(spec_file, buffer);  // get line

      // and check if it's a key - is it insde []
      begin = buffer.find_first_of("[");
      end = buffer.find_first_of("]");

      // and check if it's at least a character long
      if(begin < buffer.size() && end < buffer.size() && end - begin > 1) {
        // flush old key and values into the map
        // only make a pair if non-empty key and value
        if(!key.empty() && !end_value.empty()) {
          pairs[key] = end_value;
        }

        // start collecting new values for new key
        end_value.clear();

        // save new key
        key = buffer.substr(begin + 1, end - begin - 1);  // remove '[' and ']'
        stripWhitespace(key);

      } else {
        // append values to end_value string
        end = buffer.find_last_of("#");  // ignore comments

        // if # has been found then crop it out
        value = (end < buffer.size()) ? buffer.substr(0, end) : buffer;

        stripWhitespace(value);

        if(!value.empty()) {
          if(end_value.empty()) {
            end_value = value;

          } else {
            // if not empty separate values with ';'
            end_value += ";" + value;
          }
        }
      }
    }

    // last flush after the file ends
    if(!key.empty() && !end_value.empty()) {
      pairs[key] = end_value;
    }

    // finished reading the file
    spec_file.close();

  } else {
    cout << fs_filename << " - missing file, bailing" << endl;
    // kill game - no point continuing with missing files
    Game::instance()->kill();
  }

  return !pairs.empty();
}

/** @brief get the string from the file
 * parses for comments, doesn't strip whitespace, returns an empty string if no file found
 */
string getStringFromFile(const string& filename)
{
  string buffer;
  string value; // whatever is on next line except #comments
  string end_value; // multiline result
  ifstream string_file;

  string_file.open(filename.c_str());

  if(string_file.is_open()) {
    size_t end;

    while(!string_file.eof()) {
      getline(string_file, buffer);  // get line

      // ignore comments
      end = buffer.find_last_of("#");
      // if # has been found then crop it out
      value = (end < buffer.size()) ? buffer.substr(0, end) : buffer;

      // append what's left to the end_value string
      if(!value.empty()) {
        if(end_value.empty()) {
          end_value = value;

        } else {
          // if not empty concatenate
          end_value += "\n" + value;
        }
      }
    }

    // finished reading the file
    string_file.close();
  }

  return end_value;
}

/** @brief reads in parameters into an array of reals
 * from a format (1,1,1,1)
 */
void getRealSeries(vector<Real>& real_array,
                   string        a_buffer)
{
  size_t end = 0;
  string value;
  stripWhitespace(a_buffer);
  size_t open_brace = a_buffer.find_first_of("(");
  size_t close_brace = a_buffer.find_first_of(")");

  // check to see if we have '(' and ')' and anything inside them
  if(open_brace < close_brace - 1 && close_brace < a_buffer.size()) {
    // discard the brackets
    a_buffer = a_buffer.substr(open_brace + 1, close_brace - open_brace - 1);

    while(!a_buffer.empty()) {
      // values are seperated with ,
      end = a_buffer.find_first_of(",");

      // if there's a ','
      if(end < a_buffer.size()) {
        // take one value
        value = a_buffer.substr(0, end);

        // cut it from the buffer
        a_buffer = a_buffer.substr(end + 1);

      } else {
        // take the whole buffer as the last value
        value = a_buffer;
        a_buffer.clear();
      }

      stripWhitespace(value);

      // put values in the array as you find them
      if(!value.empty()) {
        real_array.push_back(getReal(value));
      }
    }
  }
}

/** @brief stick pairs of ints in an array
 */
void getIntPairArray(vector<int_pair>& int_pair_array,
                     string            a_buffer)
{
  vector<string> string_array;

  // cut ';' seperated strings into an array
  getStringArray(string_array, a_buffer);

  // need two values in a pair
  for(size_t i = 0; i < string_array.size() - 1; ++i) {
    int_pair pair_of_ints;
    pair_of_ints.first = intoInt(string_array[i]);
    ++i;
    pair_of_ints.second = intoInt(string_array[i]);

    int_pair_array.push_back(pair_of_ints);
  }

  assert(int_pair_array.size() > 0);
}

/** @brief put pairs of position enums into an array
 */
template <class T1, class T2>
void getEnumPairArrayT(vector<pair<T1, T2> >& position_pair_array,
                       string a_buffer)
{
  vector<string> string_array;

  // cut ; seperated strings into an array
  getStringArray(string_array, a_buffer);

  // need two values in a pair
  for(size_t i = 0; i < string_array.size() - 1; ++i) {
    pair<T1, T2> pair_of_enums;
    pair_of_enums.first = static_cast<T1>(getEnum(string_array[i]));
    ++i;
    pair_of_enums.second = static_cast<T2>(getEnum(string_array[i]));

    position_pair_array.push_back(pair_of_enums);
  }

  assert(position_pair_array.size() > 0);
}

/** @brief put r,b,g colours into a table (alpha = 1)
 */
void getColourArray(vector<Ogre::ColourValue>& colour_array,
                    string                     a_buffer)
{
  vector<string> string_array;

  // cut ; seperated strings into an array
  getStringArray(string_array, a_buffer);

  // need three values for a colour
  for(size_t i = 0; i < string_array.size() - 2; ++i) {
    Real r = intoReal(string_array[i]) / 255;
    ++i;
    Real g = intoReal(string_array[i]) / 255;
    ++i;
    Real b = intoReal(string_array[i]) / 255;

    colour_array.push_back(Ogre::ColourValue(r, g, b));
  }

  assert(colour_array.size() > 0);
}

/** @brief fills the vector with int but treats each character as an int so 82 is 8 and 2
 */
template <typename T> void getDigitArrayT(vector<T>& int_array,
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

template
void getEnumPairArrayT<horizontal::position, vertical::position>
  (vector<pair<horizontal::position, vertical::position> >&, string);

template
void getEnumArrayT<mfd_view::view_type>
  (vector<mfd_view::view_type>&, string);

}
