//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "files_handler.h"
#include "game.h"
#include "internal_type.h"
#include "crusader_design.h"
#include "hud_design.h"
#include "weapon.h"
#include "radar_computer.h"
#include "text_store.h"

//this is a massive clusterfuck of parsing and fudging and will only get worse over time
//TODO: look into replacing some of the building blocks with boost?

const string game_text_file = "game_text";

//this is temporary solution for storing the string keys
//theoretically you only need this when reading files because internally I use enums as keys
//if this gets big (unlikely) this could be removed
map<string, ulint> FilesHandler::string_key_map;

//eveything is static anyway so is there a point to this singleton? might just use a namespace
/** singleton
  */
FilesHandler* FilesHandler::instance()
{
    return inst;
}

FilesHandler* FilesHandler::inst = 0;

void FilesHandler::instantiate()
{
    inst = new FilesHandler();
}

/** @brief string to float with additional sanitation
  */
Ogre::Real FilesHandler::getReal(string a_string)
{
    //basic sanitation, strip non numbers
    for(unsigned int b = 0, for_size = a_string.size(); b < for_size; ++b) {
        if(!isdigit(a_string.at(b))&&!ispunct(a_string.at(b))) {
            a_string.erase(b, 1);
        }
    }

    return Game::stringIntoReal(a_string);
}

/** @brief true/false string to bool
  */
bool FilesHandler::getBool(string a_string)
{
    return (a_string == "true");
}

/** @brief translate string key into int key
  */
ulint FilesHandler::getStringKey(const string& a_string)
{
    ulint key = 0;

    map<string, ulint>::iterator it = string_key_map.find(a_string);

    if (it != string_key_map.end())
        key = it->second;

    return key;
}

/** @brief string to int later to be cast into an enum,
  * I know it's a bit ugly if it gets too long it can be seperated into seperate enums
  * TODO: it's getting long, if you start using it outside of files ffs split it up
  */
int FilesHandler::getEnum(const string& a_string)
{
    int enum_int = -1;

    if (a_string == "drive_type_limbs") //drive_type
        enum_int = 0;
    else if (a_string == "drive_type_wheels")
        enum_int = 1;
    else if (a_string == "drive_type_tracks")
        enum_int = 2;
    else if (a_string == "drive_type_float")
        enum_int = 3;
    else if (a_string == "drive_type_hover")
        enum_int = 4;
    else if (a_string == "drive_subtype_biped") //drive_subtype
        enum_int = 0;
    else if (a_string == "drive_subtype_quadruped")
        enum_int = 1;
    else if (a_string == "drive_subtype_multiped")
        enum_int = 2;
    else if (a_string == "drive_subtype_bicycle")
        enum_int = 3;
    else if (a_string == "drive_subtype_tricycle")
        enum_int = 4;
    else if (a_string == "drive_subtype_quad")
        enum_int = 5;
    else if (a_string == "drive_subtype_multi_wheel")
        enum_int = 6;
    else if (a_string == "drive_subtype_unitrack")
        enum_int = 7;
    else if (a_string == "drive_subtype_tracks")
        enum_int = 8;
    else if (a_string == "drive_subtype_multitracks")
        enum_int = 9;
    else if (a_string == "drive_subtype_hovercraft")
        enum_int = 10;
    else if (a_string == "drive_subtype_boat")
        enum_int = 11;
    else if (a_string == "drive_subtype_rocket")
        enum_int = 12;
    else if (a_string == "crusader_type_recon") //crusader_type
        enum_int = 0;
    else if (a_string == "crusader_type_infantry")
        enum_int = 1;
    else if (a_string == "crusader_type_assault")
        enum_int = 2;
    else if (a_string == "crusader_type_goliath")
        enum_int = 3;
    else if (a_string == "weapon_type_cannon") //weapon_type
        enum_int = 0;
    else if (a_string == "weapon_type_launcher_missile")
        enum_int = 1;
    else if (a_string == "weapon_type_energy")
        enum_int = 2;
    else if (a_string == "weapon_type_launcher")
        enum_int = 3;
    else if (a_string == "armour_type_composite") //armour_type
        enum_int = 0;
    else if (a_string == "armour_type_reactive")
        enum_int = 1;
    else if (a_string == "armour_type_plastic")
        enum_int = 2;
    else if (a_string == "armour_type_ceramic")
        enum_int = 3;
    else if (a_string == "armour_type_tesla")
        enum_int = 4;
    else if (a_string == "internal_type_struts") //internal_type
        enum_int = 0;
    else if (a_string == "internal_type_structure")
        enum_int = 1;
    else if (a_string == "internal_type_heatsink")
        enum_int = 2;
    else if (a_string == "internal_type_actuator")
        enum_int = 3;
    else if (a_string == "internal_type_gyro")
        enum_int = 4;
    else if (a_string == "internal_type_coolant")
        enum_int = 5;
    else if (a_string == "internal_type_ultra_gyro")
        enum_int = 6;
    else if (a_string == "internal_type_capacitor")
        enum_int = 7;
    else if (a_string == "torso") //body part
        enum_int = 0;
    else if (a_string == "torso_right")
        enum_int = 1;
    else if (a_string == "torso_left")
        enum_int = 2;
    else if (a_string == "arm_right")
        enum_int = 3;
    else if (a_string == "arm_left")
        enum_int = 4;
    else if (a_string == "leg_right")
        enum_int = 5;
    else if (a_string == "leg_left")
        enum_int = 6;
    else if (a_string == "leg_right_back")
        enum_int = 7;
    else if (a_string == "leg_left_back")
        enum_int = 8;
    else if (a_string == "left") //horizontal::position
        enum_int = 0;
    else if (a_string == "centre")
        enum_int = 1;
    else if (a_string == "right")
        enum_int = 2;
    else if (a_string == "top") //vertical::position
        enum_int = 0;
    //else if (a_string == "centre") already handled above
    //    enum_int = 1;
    else if (a_string == "bottom")
        enum_int = 2;
    else if (a_string == "hud_mfd1") //hud area
        enum_int = 0;
    else if (a_string == "hud_mfd2")
        enum_int = 1;
    else if (a_string == "hud_radar")
        enum_int = 2;
    else if (a_string == "hud_weapon")
        enum_int = 3;
    else if (a_string == "hud_heat")
        enum_int = 4;
    else if (a_string == "hud_speed")
        enum_int = 5;
    else if (a_string == "hud_status")
        enum_int = 6;
    else if (a_string == "hud_log")
        enum_int = 7;
    else if (a_string == "mfd") //hud part function
        enum_int = 0;
    else if (a_string == "mfd_aux")
        enum_int = 1;
    else if (a_string == "radar")
        enum_int = 2;
    else if (a_string == "radar_power")
        enum_int = 3;
    else if (a_string == "clock_tick")
        enum_int = 4;
    else if (a_string == "direction")
        enum_int = 5;
    else if (a_string == "torso_direction")
        enum_int = 6;
    else if (a_string == "compass")
        enum_int = 7;
    else if (a_string == "clock")
        enum_int = 8;
    else if (a_string == "radar_range")
        enum_int = 9;
    else if (a_string == "weapon_group")
        enum_int = 10;
    else if (a_string == "weapon_target")
        enum_int = 11;
    else if (a_string == "weapon_auto")
        enum_int = 12;
    else if (a_string == "weapons")
        enum_int = 13;
    else if (a_string == "log")
        enum_int = 14;
    else if (a_string == "temperature")
        enum_int = 15;
    else if (a_string == "temperature_external")
        enum_int = 16;
    else if (a_string == "coolant")
        enum_int = 17;
    else if (a_string == "speed")
        enum_int = 18;
    else if (a_string == "throttle")
        enum_int = 19;
    else if (a_string == "pressure")
        enum_int = 20;
    else if (a_string == "temperature_engine")
        enum_int = 21;
    else if (a_string == "status")
        enum_int = 22;
    else if (a_string == "mfd_military") //hud part type
        enum_int = 0;
    else if (a_string == "mfd_aux_military")
        enum_int = 1;
    else if (a_string == "radar_military")
        enum_int = 2;
    else if (a_string == "set")
        enum_int = 3;
    else if (a_string == "pointer")
        enum_int = 4;
    else if (a_string == "text")
        enum_int = 5;
    else if (a_string == "weapon_tab")
        enum_int = 6;
    else if (a_string == "log_printer")
        enum_int = 7;
    else if (a_string == "dial")
        enum_int = 8;
    else if (a_string == "status_display")
        enum_int = 9;
    else if (a_string == "damage_diagram_self") //mfd view types
        enum_int = 0;
    else if (a_string == "damage_diagram_target")
        enum_int = 1;
    else if (a_string == "damage_view_target")
        enum_int = 2;
    else if (a_string == "inspector")
        enum_int = 3;
    else if (a_string == "minimap")
        enum_int = 4;
    else if (a_string == "enemy_list")
        enum_int = 5;
    else if (a_string == "systems_control")
        enum_int = 6;
    else if (a_string == "squad_tactic")
        enum_int = 7;
    else if (a_string == "rotating") //radar
        enum_int = 0;
    else if (a_string == "fixed")
        enum_int = 1;
    else if (a_string == "oscilating")
        enum_int = 2;
    else //if all fails pring for debug
        cout << "malformed enum: " << a_string << endl;

    assert(enum_int > -1);

    return enum_int;
}

/** @brief fills structs with strings read from a file
  */
void FilesHandler::getStringArray(vector<string>& string_array, string a_buffer)
{
    unsigned int end = 0;
    string value;

    while (!a_buffer.empty()) {
        //semicolon seperated values
        end = a_buffer.find_first_of(";");

        //if there's a ';'
        if (end < a_buffer.size()) {
            //take one value
            value = a_buffer.substr(0, end);

            //cut it from the buffer
            a_buffer = a_buffer.substr(end + 1);

        } else {
            //take the whole buffer as the last value
            value = a_buffer;
            a_buffer.clear();
        }

        stripWhitespace(value); //skip leading and trailing whitespace

        //put values in the array as you find them
        if (!value.empty()) {
            string_array.push_back(value);
        }
    }
}

/** @brief fills structs with ints read from a file and casts them to enum
  */
template <typename T> void FilesHandler::getEnumArray(vector<T>& enum_array, string a_buffer)
{
    vector<string> string_array;

    //get strings into the vector
    getStringArray(string_array, a_buffer);

    //and turn them into enums
    for(unsigned int i = 0, for_size = string_array.size(); i < for_size; ++i) {
        enum_array.push_back(static_cast<T>(getEnum(string_array[i])));
    }
}

/** @brief fills structs with ints read from a file
  */
template <typename T> void FilesHandler::getIntArray(vector<T>& int_array, string a_buffer)
{
    unsigned int end = 0;
    string value;
    stripWhitespace(a_buffer);
    while (!a_buffer.empty()) {
        end = a_buffer.find_first_of(";"); //semicolon seperated values

        //if there's a ';'
        if (end < a_buffer.size()) {
            //take one value
            value = a_buffer.substr(0, end);

            //cut it from the buffer
            a_buffer = a_buffer.substr(end + 1);

        } else {
            //take the whole buffer as the last value
            value = a_buffer;
            a_buffer.clear();
        }

        //insert values as you find them
        if (!value.empty()) {
            int_array.push_back(Game::stringIntoInt(value));
        }
    }
}

/** @brief get pairs of strings form a file
  * it's the workhorse of parsing files into a map for most functions - parses file of a format:
  * [key] #comments
  * value #comments
  * and returns pairs of strings
  */
bool FilesHandler::getPairs(const string& filename, const string& dir, map<string, string>& pairs)
{
    string buffer;
    string key; //[key] ignore whatver is after ]
    string value; //whatever is on next line except #comments
    string end_value; //multiline result
    string fs_filename = data_dir+dir+filename;
    ifstream spec_file;

    spec_file.open(fs_filename.c_str());

    if (spec_file.is_open()) {
        unsigned int begin;
        unsigned int end;

        while (! spec_file.eof() ) {
            getline(spec_file, buffer); //get line

            //and check if it's a key - is it insde []
            begin = buffer.find_first_of("[");
            end = buffer.find_first_of("]");

            //and check if it's at least a character long
            if (begin < buffer.size() && end < buffer.size() && end - begin > 1) {
                //flush old key and values into the map
                //only make a pair if non-empty key and value
                if (!key.empty() && !end_value.empty()) {
                    pairs[key] = end_value;
                }

                //start collecting new values for new key
                end_value.clear();

                //save new key
                key = buffer.substr(begin + 1, end - begin - 1); //remove '[' and ']'
                stripWhitespace(key);

            } else {
                //append values to end_value string
                end = buffer.find_last_of("#"); //ignore comments

                //if # has been found then crop it out
                value = (end < buffer.size())? buffer.substr(0, end) : buffer;

                stripWhitespace(value);

                if (!value.empty()) {
                    if (end_value.empty()) {
                        end_value = value;

                    } else {
                        //if not empty separate values with ';'
                        end_value += ";"+value;
                    }
                }
            }
        }

        //last flush after the file ends
        if (!key.empty() && !end_value.empty()) {
            pairs[key] = end_value;
        }

        //finished reading the file
        spec_file.close();

    } else {
        cout << fs_filename << " - missing file, bailing" << endl;
        //kill game - no point continuing with missing files
        Game::instance()->kill();
    }

    return !pairs.empty();
}

/** @brief get the string from the file
  * parses for comments, doesn't strip whitespace, returns an empty string if no file found
  */
string FilesHandler::getStringFromFile(const string& filename)
{
    string buffer;
    string value; //whatever is on next line except #comments
    string end_value; //multiline result
    ifstream string_file;

    string_file.open(filename.c_str());

    if (string_file.is_open()) {
        unsigned int end;

        while (! string_file.eof() ) {
            getline(string_file, buffer); //get line

            //ignore comments
            end = buffer.find_last_of("#");
            //if # has been found then crop it out
            value = (end < buffer.size())? buffer.substr(0, end) : buffer;

            //append what's left to the end_value string
            if (!value.empty()) {
                if (end_value.empty()) {
                    end_value = value;

                } else {
                    //if not empty concatenate
                    end_value += "\n"+value;
                }
            }
        }

        //finished reading the file
        string_file.close();
    }

    return end_value;
}

/** @brief fills the vector with game text
  * whilst doing this also creates a map for matching strings to numerical keys used internally
  */
bool FilesHandler::getGameText(vector<string>& game_text)
{
    string buffer;
    string key; //[key] ignore whatver is after ]
    string value; //whatever is on next line except #comments
    string end_value; //multiline result
    string fs_filename = data_dir+text_dir+game_text_file;
    ifstream spec_file;
    ulint string_n = 0; //number of keys insrted

    spec_file.open(fs_filename.c_str());

    if (spec_file.is_open()) {
        unsigned int begin;
        unsigned int end;

        while (! spec_file.eof() ) {
            //go line by line
            getline(spec_file, buffer);

            //check if it's a key - is it insde []?
            begin = buffer.find_first_of("[");
            end = buffer.find_first_of("]");

            //and check if it's at least one character long
            if (begin < buffer.size() && end < buffer.size() && end - begin > 1) {
                //flush old key and values into vectors if they both exist
                if (!key.empty() && !end_value.empty()) {
                    game_text.push_back(end_value);
                    string_key_map[key] = string_n;
                    ++string_n;
                }

                //start collecting new values for new key
                end_value.clear();

                //remove '[' and ']' and save new key
                key = buffer.substr(begin + 1, end - begin - 1);
                stripWhitespace(key); //remove whitespace if any

            } else {
                //if it's not a key append values to end_value string

                //ignore comments
                end = buffer.find_last_of("#");

                //if # has been found then crop it out
                value = (end < buffer.size())? buffer.substr(0, end) : buffer;

                if (!value.empty()) {
                    if (end_value.empty()) { //if there's nothing yetm just copy the value
                        end_value = value;

                    } else { //there are multiple lines in the string put in newlines between values
                        end_value += "\n"+value;
                    }
                }
            }
        }

        //last flush after the file ends
        if (!key.empty() && !end_value.empty()) {
            game_text.push_back(end_value);
            string_key_map[key] = string_n;
            ++string_n;
        }

        //finished reading file
        spec_file.close();

    } else {
        cout << fs_filename << " - missing file, bailing" << endl;

        //kill game - no point continuing with missing files
        Game::instance()->kill();
    }

    return !game_text.empty();
}

/** @brief strips leading and trailing spaces
  */
void FilesHandler::stripWhitespace(string& a_string)
{
    if (!a_string.empty()) {
        unsigned int begin = a_string.find_first_not_of(" ");
        unsigned int end = a_string.find_last_not_of(" ");
        a_string = a_string.substr(begin, end - begin + 1);
    }
}

/** @brief translates from human readable to OIS ids
  */
OIS::KeyCode FilesHandler::translateKeycode(const string& id)
{
    map<string, OIS::KeyCode> keys;

    //build map to translate string to OIS keycode
    keys["ESCAPE"] = OIS::KC_ESCAPE;
    keys["1"] = OIS::KC_1;
    keys["2"] = OIS::KC_2;
    keys["3"] = OIS::KC_3;
    keys["4"] = OIS::KC_4;
    keys["5"] = OIS::KC_5;
    keys["6"] = OIS::KC_6;
    keys["7"] = OIS::KC_7;
    keys["8"] = OIS::KC_8;
    keys["9"] = OIS::KC_9;
    keys["0"] = OIS::KC_0;
    keys["Q"] = OIS::KC_Q;
    keys["W"] = OIS::KC_W;
    keys["E"] = OIS::KC_E;
    keys["R"] = OIS::KC_R;
    keys["T"] = OIS::KC_T;
    keys["Y"] = OIS::KC_Y;
    keys["U"] = OIS::KC_U;
    keys["I"] = OIS::KC_I;
    keys["O"] = OIS::KC_O;
    keys["P"] = OIS::KC_P;
    keys["A"] = OIS::KC_A;
    keys["S"] = OIS::KC_S;
    keys["D"] = OIS::KC_D;
    keys["F"] = OIS::KC_F;
    keys["G"] = OIS::KC_G;
    keys["H"] = OIS::KC_H;
    keys["J"] = OIS::KC_J;
    keys["K"] = OIS::KC_K;
    keys["L"] = OIS::KC_L;
    keys["Z"] = OIS::KC_Z;
    keys["X"] = OIS::KC_X;
    keys["C"] = OIS::KC_C;
    keys["V"] = OIS::KC_V;
    keys["B"] = OIS::KC_B;
    keys["N"] = OIS::KC_N;
    keys["M"] = OIS::KC_M;
    keys["F1"] = OIS::KC_F1;
    keys["F2"] = OIS::KC_F2;
    keys["F3"] = OIS::KC_F3;
    keys["F4"] = OIS::KC_F4;
    keys["F5"] = OIS::KC_F5;
    keys["F6"] = OIS::KC_F6;
    keys["F7"] = OIS::KC_F7;
    keys["F8"] = OIS::KC_F8;
    keys["F9"] = OIS::KC_F9;
    keys["F10"] = OIS::KC_F10;
    keys["LBRACKET"] = OIS::KC_LBRACKET;
    keys["RBRACKET"] = OIS::KC_RBRACKET;
    keys["RETURN"] = OIS::KC_RETURN;
    keys["LCONTROL"] = OIS::KC_LCONTROL;
    keys["MINUS"] = OIS::KC_MINUS;
    keys["EQUALS"] = OIS::KC_EQUALS;
    keys["BACK"] = OIS::KC_BACK;
    keys["TAB"] = OIS::KC_TAB;
    keys["SEMICOLON"] = OIS::KC_SEMICOLON;
    keys["APOSTROPHE"] = OIS::KC_APOSTROPHE;
    keys["GRAVE"] = OIS::KC_GRAVE;
    keys["LSHIFT"] = OIS::KC_LSHIFT;
    keys["BACKSLASH"] = OIS::KC_BACKSLASH;
    keys["COMMA"] = OIS::KC_COMMA;
    keys["PERIOD"] = OIS::KC_PERIOD;
    keys["SLASH"] = OIS::KC_SLASH;
    keys["RSHIFT"] = OIS::KC_RSHIFT;
    keys["MULTIPLY"] = OIS::KC_MULTIPLY;
    keys["LMENU"] = OIS::KC_LMENU;
    keys["SPACE"] = OIS::KC_SPACE;
    keys["CAPITAL"] = OIS::KC_CAPITAL;
    keys["NUMLOCK"] = OIS::KC_NUMLOCK; //numpad
    keys["NUMPADCOMMA"] = OIS::KC_NUMPADCOMMA;
    keys["SCROLL"] = OIS::KC_SCROLL;
    keys["NUMPAD7"] = OIS::KC_NUMPAD7;
    keys["NUMPAD8"] = OIS::KC_NUMPAD8;
    keys["NUMPAD9"] = OIS::KC_NUMPAD9;
    keys["SUBTRACT"] = OIS::KC_SUBTRACT;
    keys["NUMPAD4"] = OIS::KC_NUMPAD4;
    keys["NUMPAD5"] = OIS::KC_NUMPAD5;
    keys["NUMPAD6"] = OIS::KC_NUMPAD6;
    keys["ADD"] = OIS::KC_ADD;
    keys["NUMPAD1"] = OIS::KC_NUMPAD1;
    keys["NUMPAD2"] = OIS::KC_NUMPAD2;
    keys["NUMPAD3"] = OIS::KC_NUMPAD3;
    keys["NUMPAD0"] = OIS::KC_NUMPAD0;
    keys["DECIMAL"] = OIS::KC_DECIMAL;
    keys["RCONTROL"] = OIS::KC_RCONTROL;
    keys["DIVIDE"] = OIS::KC_DIVIDE;
    keys["SYSRQ"] = OIS::KC_SYSRQ;
    keys["RMENU"] = OIS::KC_RMENU;
    keys["PAUSE"] = OIS::KC_PAUSE;
    keys["HOME"] = OIS::KC_HOME;
    keys["UP"] = OIS::KC_UP;
    keys["PGUP"] = OIS::KC_PGUP;
    keys["LEFT"] = OIS::KC_LEFT;
    keys["RIGHT"] = OIS::KC_RIGHT;
    keys["END"] = OIS::KC_END;
    keys["DOWN"] = OIS::KC_DOWN;
    keys["PGDOWN"] = OIS::KC_PGDOWN;
    keys["INSERT"] = OIS::KC_INSERT;
    keys["DELETE"] = OIS::KC_DELETE;
    keys["LWIN"] = OIS::KC_LWIN;
    keys["RWIN"] = OIS::KC_RWIN;
    keys["APPS"] = OIS::KC_APPS;

    //return unassaigned if key is not on the list
    if (keys.find(id) == keys.end()) {
        return OIS::KC_UNASSIGNED;

    } else {
        return keys[id];
    }
}

/** @brief translates from human readable to OIS ids
  */
OIS::MouseButtonID FilesHandler::translateMouse(const string& id)
{
    map<string, OIS::MouseButtonID> mouse;

    //build map to translate string to OIS mouse id
    mouse["MOUSE_LEFT"] = OIS::MB_Left;
    mouse["MOUSE_MIDDLE"] = OIS::MB_Middle;
    mouse["MOUSE_RIGHT"] = OIS::MB_Right;
    mouse["MOUSE_BUTTON3"] = OIS::MB_Button3;
    mouse["MOUSE_BUTTON4"] = OIS::MB_Button4;
    mouse["MOUSE_BUTTON5"] = OIS::MB_Button5;
    mouse["MOUSE_BUTTON6"] = OIS::MB_Button6;

    //return button7 which is neer used if id is not on the list
    if (mouse.find(id) == mouse.end()) {
        return OIS::MB_Button7;

    } else {
        return mouse[id];
    }
}

/** @brief reads in parameters into an array of reals
  * from a format (1,1,1,1)
  */
void FilesHandler::getRealSeries(vector<Ogre::Real>& real_array, string a_buffer)
{
    unsigned int end = 0;
    string value;
    stripWhitespace(a_buffer);
    unsigned int open_brace = a_buffer.find_first_of("(");
    unsigned int close_brace = a_buffer.find_first_of(")");

    //check to see if we have '(' and ')' and anything inside them
    if (open_brace < close_brace - 1 && close_brace < a_buffer.size()) {
        //discard the brackets
        a_buffer = a_buffer.substr(open_brace + 1, close_brace - open_brace - 1);

        while (!a_buffer.empty()) {
            //values are seperated with ,
            end = a_buffer.find_first_of(",");

            //if there's a ','
            if (end < a_buffer.size()) {
                //take one value
                value = a_buffer.substr(0, end);

                //cut it from the buffer
                a_buffer = a_buffer.substr(end + 1);

            } else {
                //take the whole buffer as the last value
                value = a_buffer;
                a_buffer.clear();
            }

            stripWhitespace(value);

            //put values in the array as you find them
            if (!value.empty()) {
                real_array.push_back(getReal(value));
            }
        }
    }
}

/** @brief propagates the last colour in a table if there are too few
  */
void FilesHandler::padHudColours(vector<Ogre::ColourValue>& colour_array)
{
    //if fewer colours than hud needs
    if (colour_array.size() < hud_num_of_colours) {
        //copy the last colour over and over until full
        for (usint i = colour_array.size(); i < hud_num_of_colours; ++i) {
            colour_array.push_back(colour_array.back());
        }
    }
}

/** @brief stick pairs of ints in an array
  */
void FilesHandler::getIntPairArray(vector<int_pair>& int_pair_array, string a_buffer)
{
    vector<string> string_array;

    //cut ';' seperated strings into an array
    getStringArray(string_array, a_buffer);

    //need two values in a pair
    for(unsigned int i = 0; i < string_array.size() - 1; ++i) {
        int_pair pair_of_ints;
        pair_of_ints.first = Game::stringIntoInt(string_array[i]);
        ++i;
        pair_of_ints.second = Game::stringIntoInt(string_array[i]);

        int_pair_array.push_back(pair_of_ints);
    }

    assert(int_pair_array.size() > 0);
}

/** @brief put pairs of position enums into an array
  */
template <class T1, class T2>
void FilesHandler::getEnumPairArray(vector<pair<T1, T2> >& position_pair_array, string a_buffer)
{
    vector<string> string_array;

    //cut ; seperated strings into an array
    getStringArray(string_array, a_buffer);

    //need two values in a pair
    for(unsigned int i = 0; i < string_array.size() - 1; ++i) {
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
void FilesHandler::getColourArray(vector<Ogre::ColourValue>& colour_array, string a_buffer)
{
    vector<string> string_array;

    //cut ; seperated strings into an array
    getStringArray(string_array, a_buffer);

    //need three values for a colour
    for(unsigned int i = 0; i < string_array.size() - 2; ++i) {
        Ogre::Real r = Game::stringIntoReal(string_array[i]) / 255;
        ++i;
        Ogre::Real g = Game::stringIntoReal(string_array[i]) / 255;
        ++i;
        Ogre::Real b = Game::stringIntoReal(string_array[i]) / 255;

        colour_array.push_back(Ogre::ColourValue(r, g, b));
    }

    assert(colour_array.size() > 0);
}

/** @brief loads collision spheres from a file
  */
bool FilesHandler::getCollisionSpheres(const string& filename, Sphere& bounding_sphere,
                                       vector<Sphere>& exclusion_spheres,
                                       vector<bitset<max_num_cs> >& exclusion_bitsets,
                                       vector<usint>& es_areas,
                                       vector<Sphere>&  collision_spheres, vector<usint>& cs_areas)
{
    map<string, string> pairs;
    assert(getPairs(filename, model_dir, pairs));

    //bounding sphere
    vector<string> bs_sphere_string;
    getStringArray(bs_sphere_string, pairs["bs"]);

    //makes sure the are enough values
    if (bs_sphere_string.size() < 4) {
        cout << "bounding sphere sphere in ";
        return false;
    }

    //read in the bounding sphere
    bounding_sphere.centre = Ogre::Vector3(getReal(bs_sphere_string[0]),
                                           getReal(bs_sphere_string[1]),
                                           getReal(bs_sphere_string[2]));
    bounding_sphere.radius = getReal(bs_sphere_string[3]);

    Sphere sphere;
    int i = 0;

    //check to see if a exclusion sphere with a consecutive id exists
    while (pairs.find(string("es.")+Game::intIntoString(i)) != pairs.end() && i < max_num_es) {
        //load the es definition string
        vector<string> es_sphere_string;
        getStringArray(es_sphere_string, pairs["es."+Game::intIntoString(i)]);

        //makes sure the are enough values
        if (es_sphere_string.size() < 5) {
            cout << "exclusion sphere sphere in ";
            return false;
        }

        //exclusion sphere area (to move the sphere when it moves)
        es_areas.push_back(Game::stringIntoInt(es_sphere_string[1]));

        //read in the exclusion sphere
        sphere.centre = Ogre::Vector3(getReal(es_sphere_string[1]),
                                      getReal(es_sphere_string[2]),
                                      getReal(es_sphere_string[3]));
        sphere.radius = getReal(es_sphere_string[4]);
        //put the sphere in the vector
        exclusion_spheres.push_back(sphere);

        //create an empty bitset for the sphere
        exclusion_bitsets.push_back(bitset<max_num_cs>(0));

        ++i;
    }

    i = 0;

    //check to see if a collision sphere with a consecutive id exists
    while (pairs.find(string("cs.")+Game::intIntoString(i)) != pairs.end() && i < max_num_cs) {
        //load the cs definition string
        vector<string> cs_sphere_string;
        getStringArray(cs_sphere_string, pairs["cs."+Game::intIntoString(i)]);

        //makes sure the are enough values
        if (cs_sphere_string.size() < 6) {
            cout << "collision sphere in ";
            return false;
        }

        //collision sphere area (to place hits)
        cs_areas.push_back(Game::stringIntoInt(cs_sphere_string[0]));

        //set exclusion spheres' bitsets
        for (usint j = 0, for_size = exclusion_bitsets.size(); j < for_size; ++j) {
            //if the bit is set for this es then set the bit for the cs in the es bitset
            if (cs_sphere_string[1][j] == '1') {
                exclusion_bitsets[j][i] = 1;
            }
        }

        //read in the collision sphere
        sphere.centre = Ogre::Vector3(getReal(cs_sphere_string[2]),
                                      getReal(cs_sphere_string[3]),
                                      getReal(cs_sphere_string[4]));
        sphere.radius = getReal(cs_sphere_string[5]);
        //put the sphere in the vector
        collision_spheres.push_back(sphere);

        ++i;
    }

    return true;
}

/** @brief loads the config from a file
  */
bool FilesHandler::getKeyConfig(const string& filename, map<input_event, OIS::KeyCode>& key_map,
                                map<input_event, OIS::MouseButtonID>& mouse_map)
{
    //insert data from file into pairs
    map<string, string> pairs;
    assert(getPairs(filename, config_dir, pairs));
    map<input_event, string> enum_pairs;

    //create a map to translate enum to string which indexes the pairs from the file
    enum_pairs.insert(make_pair(input_event_up, "input_event_up"));
    enum_pairs.insert(make_pair(input_event_down, "input_event_down"));
    enum_pairs.insert(make_pair(input_event_left, "input_event_left"));
    enum_pairs.insert(make_pair(input_event_right, "input_event_right"));
    enum_pairs.insert(make_pair(input_event_set_speed_reverse, "input_event_set_speed_reverse"));
    enum_pairs.insert(make_pair(input_event_set_speed_0, "input_event_set_speed_0"));
    enum_pairs.insert(make_pair(input_event_set_speed_1, "input_event_set_speed_1"));
    enum_pairs.insert(make_pair(input_event_set_speed_2, "input_event_set_speed_2"));
    enum_pairs.insert(make_pair(input_event_set_speed_3, "input_event_set_speed_3"));
    enum_pairs.insert(make_pair(input_event_set_speed_4, "input_event_set_speed_4"));
    enum_pairs.insert(make_pair(input_event_set_speed_5, "input_event_set_speed_5"));
    enum_pairs.insert(make_pair(input_event_set_speed_6, "input_event_set_speed_6"));
    enum_pairs.insert(make_pair(input_event_set_speed_7, "input_event_set_speed_7"));
    enum_pairs.insert(make_pair(input_event_set_speed_8, "input_event_set_speed_8"));
    enum_pairs.insert(make_pair(input_event_set_speed_9, "input_event_set_speed_9"));
    enum_pairs.insert(make_pair(input_event_set_speed_10, "input_event_set_speed_10"));
    enum_pairs.insert(make_pair(input_event_fire, "input_event_fire"));
    enum_pairs.insert(make_pair(input_event_turn_to_pointer, "input_event_turn_to_pointer"));
    enum_pairs.insert(make_pair(input_event_crouch, "input_event_crouch"));
    enum_pairs.insert(make_pair(input_event_align_to_torso, "input_event_align_to_torso"));
    enum_pairs.insert(make_pair(input_event_nearest_enemy, "input_event_nearest_enemy"));
    enum_pairs.insert(make_pair(input_event_enemy, "input_event_enemy"));
    enum_pairs.insert(make_pair(input_event_enemy_back, "input_event_enemy_back"));
    enum_pairs.insert(make_pair(input_event_friendly, "input_event_friendly"));
    enum_pairs.insert(make_pair(input_event_friendly_back, "input_event_friendly_back"));
    enum_pairs.insert(make_pair(input_event_target_pointer, "input_event_target_pointer"));
    enum_pairs.insert(make_pair(input_event_nav_point, "input_event_nav_point"));
    enum_pairs.insert(make_pair(input_event_nav_point_back, "input_event_nav_point_back"));
    enum_pairs.insert(make_pair(input_event_inspect, "input_event_inspect"));
    enum_pairs.insert(make_pair(input_event_fire_mode_group, "input_event_fire_mode_group"));
    enum_pairs.insert(make_pair(input_event_auto_cycle, "input_event_auto_cycle"));
    enum_pairs.insert(make_pair(input_event_cycle_weapon, "input_event_cycle_weapon"));
    enum_pairs.insert(make_pair(input_event_cycle_group, "input_event_cycle_group"));
    enum_pairs.insert(make_pair(input_event_fire_group_1, "input_event_fire_group_1"));
    enum_pairs.insert(make_pair(input_event_fire_group_2, "input_event_fire_group_2"));
    enum_pairs.insert(make_pair(input_event_fire_group_3, "input_event_fire_group_3"));
    enum_pairs.insert(make_pair(input_event_fire_group_4, "input_event_fire_group_4"));
    enum_pairs.insert(make_pair(input_event_fire_group_5, "input_event_fire_group_5"));
    enum_pairs.insert(make_pair(input_event_fire_group_all, "input_event_fire_group_all"));
    enum_pairs.insert(make_pair(input_event_target_low, "input_event_target_low"));
    enum_pairs.insert(make_pair(input_event_target_high, "input_event_target_high"));
    enum_pairs.insert(make_pair(input_event_target_air, "input_event_target_air"));
    enum_pairs.insert(make_pair(input_event_power, "input_event_power"));
    enum_pairs.insert(make_pair(input_event_radar, "input_event_radar"));
    enum_pairs.insert(make_pair(input_event_flush_coolant, "input_event_flush_coolant"));
    enum_pairs.insert(make_pair(input_event_lights, "input_event_lights"));
    enum_pairs.insert(make_pair(input_event_nightvision, "input_event_nightvision"));
    enum_pairs.insert(make_pair(input_event_zoom_in, "input_event_zoom_in"));
    enum_pairs.insert(make_pair(input_event_zoom_out, "input_event_zoom_out"));
    enum_pairs.insert(make_pair(input_event_zoom_target, "input_event_zoom_target"));
    enum_pairs.insert(make_pair(input_event_menu_interface, "input_event_menu_interface"));
    enum_pairs.insert(make_pair(input_event_communication_interface,
                                "input_event_communication_interface"));
    enum_pairs.insert(make_pair(input_event_pause, "input_event_pause"));
    enum_pairs.insert(make_pair(input_event_radar_zoom_in, "input_event_radar_zoom_in"));
    enum_pairs.insert(make_pair(input_event_radar_zoom_out, "input_event_radar_zoom_out"));
    enum_pairs.insert(make_pair(input_event_mfd1_select, "input_event_mfd1_select"));
    enum_pairs.insert(make_pair(input_event_mfd2_select, "input_event_mfd2_select"));
    enum_pairs.insert(make_pair(input_event_mfd_up, "input_event_mfd_up"));
    enum_pairs.insert(make_pair(input_event_mfd_down, "input_event_mfd_down"));
    enum_pairs.insert(make_pair(input_event_mfd_left, "input_event_mfd_left"));
    enum_pairs.insert(make_pair(input_event_mfd_right, "input_event_mfd_right"));
    enum_pairs.insert(make_pair(input_event_mfd_toggle, "input_event_mfd_toggle"));
    enum_pairs.insert(make_pair(input_event_log, "input_event_log"));

    //iterate through all input enums and insert keycodes or mouseids to key_map and mouse_map
    for (unsigned int i = 0, for_size = enum_pairs.size(); i < for_size; ++i) {
        //get OIS keycode
        OIS::KeyCode key = translateKeycode(pairs[enum_pairs[input_event(i)]]);

        //if it's a valid key
        if (key != OIS::KC_UNASSIGNED) {
            //assign it to the input event
            key_map.insert(make_pair(input_event(i), key));
            //disable mouse button
            mouse_map.insert(make_pair(input_event(i), OIS::MB_Button7));

        } else {
            //otherwise get a mouse id
            OIS::MouseButtonID mouseid = translateMouse(pairs[enum_pairs[input_event(i)]]);

            //if it's a valid mouse id
            if (mouseid != OIS::MB_Button7) {
                //assign it to the input event
                mouse_map.insert(make_pair(input_event(i), mouseid));
                //disable key
                key_map.insert(make_pair(input_event(i), OIS::KC_UNASSIGNED));

            } else {
                //TODO: fallback to default config;
                assert(0 == "the key map is garbled");
                return false;
            }
        }
    }

    return true;
}

/** @brief load weapon spec from file
  */
bool FilesHandler::getWeaponDesign(const string& filename, weapon_design_t& weapon_design)
{
    //prepare map to read data into
    map<string, string> pairs;
    //insert data from file into pairs
    assert(getPairs(filename, weapon_dir, pairs));

    //fill structs with info from pairs
    weapon_design.filename = filename;
    weapon_design.model = pairs["weapon_design.model"];
    //game text
    weapon_design.text_name = getStringKey(pairs["weapon_design.text_name"]);
    weapon_design.text_list_name = getStringKey(pairs["weapon_design.text_list_name"]);
    weapon_design.text_description = getStringKey(pairs["weapon_design.text_description"]);
    //dimensions
    weapon_design.type = weapon_type(getEnum(pairs["weapon_design.type"]));
    weapon_design.weight = getReal(pairs["weapon_design.weight"]);
    weapon_design.internals = Game::stringIntoInt(pairs["weapon_design.internals"]);
    weapon_design.panels = Game::stringIntoInt(pairs["weapon_design.panels"]);
    //multifire weapons
    weapon_design.multi_fire = Game::stringIntoInt(pairs["weapon_design.multi_fire"]);
    weapon_design.multi_fire_timout = getReal(pairs["weapon_design.multi_fire_timout"]);
    //basic properties
    weapon_design.spread = getReal(pairs["weapon_design.spread"]) * pi;
    weapon_design.recharge_time = getReal(pairs["weapon_design.recharge_time"]);
    weapon_design.heat_generated = getReal(pairs["weapon_design.heat_generated"]);
    weapon_design.muzzle_velocity = getReal(pairs["weapon_design.muzzle_velocity"]);
    weapon_design.range = getReal(pairs["weapon_design.range"]);
    weapon_design.ammo_per_slot = Game::stringIntoInt(pairs["weapon_design.ammo_per_slot"]);
    //damage
    weapon_design.heat_dmg = getReal(pairs["weapon_design.heat_dmg"]);
    weapon_design.ballistic_dmg = getReal(pairs["weapon_design.ballistic_dmg"]);
    weapon_design.penetration = getReal(pairs["weapon_design.penetration"]);
    weapon_design.energy_dmg = getReal(pairs["weapon_design.energy_dmg"]);
    //exploading damage
    weapon_design.splash_range = getReal(pairs["weapon_design.splash_range"]);
    weapon_design.splash_velocity = getReal(pairs["weapon_design.splash_velocity"]);
    //projectile
    weapon_design.projectile = pairs["weapon_design.projectile"];
    weapon_design.projectile_weight = getReal(pairs["weapon_design.projectile_weight"]);
    //used mainly for missiles
    weapon_design.fuel = getReal(pairs["weapon_design.fuel"]);
    weapon_design.homing = getReal(pairs["weapon_design.homing"]);
    weapon_design.lock_on_time = getReal(pairs["weapon_design.lock_on_time"]);

    return true;
}

/** @brief load weapon spec from file
  */
bool FilesHandler::getRadarDesign(const string& filename, radar_design_t& radar_design)
{
    //prepare map to read data into
    map<string, string> pairs;
    //insert data from file into pairs
    assert(getPairs(filename, radar_dir, pairs));

    //fill structs with info from pairs
    radar_design.filename = filename;
    radar_design.model = pairs["radar_design.model"];
    //game text
    radar_design.text_name = getStringKey(pairs["radar_design.text_name"]);
    radar_design.text_list_name = getStringKey(pairs["radar_design.text_list_name"]);
    radar_design.text_description = getStringKey(pairs["radar_design.text_description"]);
    //basic properties
    radar_design.weight = getReal(pairs["radar_design.weight"]);
    //radar type
    radar_design.active = getBool(pairs["radar_design.active"]);
    radar_design.sweep = radar::sweep_type(getEnum(pairs["radar_design.sweep"]));
    radar_design.cone_angle = Ogre::Radian(getReal(pairs["radar_design.cone_angle"]) * degree2rad);
    radar_design.heads = Game::stringIntoInt(pairs["radar_design.heads"]);
    //radar parameters
    radar_design.power = getReal(pairs["radar_design.power"]);
    radar_design.heat_sensivity = getReal(pairs["radar_design.power"]);
    radar_design.electromagnetic_sensivity =
        getReal(pairs["radar_design.electromagnetic_sensivity"]);

    return true;
}

/** @brief load the customisable design of a crusader
  * fills struct with design read from a file
  */
bool FilesHandler::getCrusaderDesign(const string& filename, crusader_design_t& design,
                                     crusader_engine_t& engine, crusader_drive_t& drive,
                                     crusader_chasis_t& chasis)
{
    using namespace crusader_part;

    //prepare map to read data into
    map<string, string> pairs;
    //insert data from file into pairs
    assert(getPairs(filename, unit_dir, pairs));

    //fill structs with info from pairs
    design.filename = filename;
    design.unit = pairs["design.unit"];

    //load spec
    getCrusaderSpec(design.unit, engine, drive, chasis);
    getStringArray(design.weapons, pairs["design.weapons"]);
    vector<Ogre::Real> default_0(chasis.num_of_areas, 0); //defaults 0 armour
    design.armour_placement = default_0;

    //continue loading the design
    design.material = pairs["design.material"];
    design.hud = pairs["design.hud"];
    design.radar = pairs["design.radar"];
    getIntArray(design.weapons_extra_ammo, pairs["design.weapons_extra_ammo"]);
    getEnumArray(design.weapons_placement, pairs["design.weapons_placement"]);
    getStringArray(design.equipment, pairs["design.equipment"]);
    getEnumArray(design.internals, pairs["design.internals"]);
    getEnumArray(design.internals_placement, pairs["design.internals_placement"]);
    design.armour =
        armour_type(getEnum(pairs["design.armour"]));

    //armour placement
    design.armour_placement[crusader_area::torso] =
        getReal(pairs["design.armour_placement.torso"]);
    design.armour_placement[crusader_area::torso_right] =
        getReal(pairs["design.armour_placement.torso_right"]);
    design.armour_placement[crusader_area::torso_left] =
        getReal(pairs["design.armour_placement.torso_left"]);
    design.armour_placement[crusader_area::arm_right] =
        getReal(pairs["design.armour_placement.arm_right"]);
    design.armour_placement[crusader_area::arm_left] =
        getReal(pairs["design.armour_placement.arm_left"]);
    design.armour_placement[crusader_area::leg_right] =
        getReal(pairs["design.armour_placement.leg_right"]);
    design.armour_placement[crusader_area::leg_left] =
        getReal(pairs["design.armour_placement.leg_left"]);
    design.armour_placement[crusader_area::torso_back] =
        getReal(pairs["design.armour_placement.torso_back"]);
    design.armour_placement[crusader_area::torso_right_back] =
        getReal(pairs["design.armour_placement.torso_right_back"]);
    design.armour_placement[crusader_area::torso_left_back] =
        getReal(pairs["design.armour_placement.torso_left_back"]);

    //read weapons into weapon groups
    for (usint i = 0; i < num_of_weapon_groups; ++i) {
        stringstream i_stream;
        i_stream << "design.weapon_groups." << i;
        getIntArray(design.weapon_groups[i], pairs[i_stream.str()]);
    }

    return true;
}

/** @brief load inmutable specification of a crusader linked to the graphics
  * fills structs with specs read from a file
  */
bool FilesHandler::getCrusaderSpec(const string& filename, crusader_engine_t& engine,
                                   crusader_drive_t& drive, crusader_chasis_t& chasis)
{
    using namespace crusader_part;

    //prepare map to read data into
    map<string, string> pairs;
    //insert data from file into pairs
    assert(getPairs(filename, crusader_dir, pairs));

    //fill structs with info from pairs
    chasis.filename = filename;
    chasis.name = pairs["chasis.name"];
    chasis.model = pairs["chasis.model"];
    chasis.type = crusader_type(getEnum(pairs["chasis.type"]));
    engine.rating = getReal(pairs["engine.rating"]); //effective[KN]
    engine.rating_reverse = getReal(pairs["engine.rating_reverse"]); //effective[KN]
    engine.heat = getReal(pairs["engine.heat"]); //[MJ]
    engine.weight = getReal(pairs["engine.weight"]); //[t]
    engine.size = Game::stringIntoInt(pairs["engine.size"]); //[m3]
    drive.type = drive_type(getEnum(pairs["drive.type"]));
    drive.subtype = drive_subtype(getEnum(pairs["drive.subtype"]));
    drive.mesh = pairs["drive.mesh"];
    drive.max_speed = getReal(pairs["drive.max_speed"]); //[m/s]
    drive.max_speed_reverse = getReal(pairs["drive.max_speed_reverse"]); //[m/s]
    drive.weight = getReal(pairs["drive.weight"]); //[t]
    drive.turn_speed = getReal(pairs["drive.turn_speed"]) * pi; //[rad/s]
    drive.kinematic_resistance = getReal(pairs["drive.kinematic_resistance"]); //[KN]
    drive.kinematic_resistance_reverse = getReal(pairs["drive.kinematic_resistance_reverse"]);
    drive.traction = getReal(pairs["drive.traction"]); //- range (0, 1)

    //TODO: add more types of drives an read more parts
    if (drive.subtype == drive_subtype_quadruped) {
        chasis.num_of_parts = 9;
        chasis.num_of_areas = 12;

    } else {
        //default for most
        chasis.num_of_parts = 7;
        chasis.num_of_areas = 10;
    }
    chasis.internals.reserve(chasis.num_of_parts);
    chasis.panels.reserve(chasis.num_of_parts);
    chasis.surface_area.reserve(chasis.num_of_areas);

    //defaults all to 0
    vector<Ogre::Real> default_0_real(chasis.num_of_areas, 0);
    chasis.surface_area = default_0_real;
    vector<usint> defualt_0(chasis.num_of_parts, 0);
    chasis.internals = defualt_0;
    chasis.panels = defualt_0;

    //continue loading
    chasis.mesh = pairs["chasis.mesh"];
    chasis.weight = getReal(pairs["chasis.weight"]); //[t]
    chasis.max_weight = getReal(pairs["chasis.max_weight"]); //[t]
    chasis.heat_dissipation_base = getReal(pairs["chasis.heat_dissipation_base"]); //[MJ]
    chasis.heat_dissipation_rate = getReal(pairs["chasis.heat_dissipation_rate"]); //multpiplier
    chasis.torso_arc = getReal(pairs["chasis.torso_arc"]) * pi; //[rad]
    chasis.arms_arc = getReal(pairs["chasis.arms_arc"]) * pi; //[rad]
    chasis.torso_turn_speed = getReal(pairs["chasis.torso_turn_speed"]) * pi; //[rad/s]
    chasis.arms_turn_speed = getReal(pairs["chasis.arms_turn_speed"]) * pi; //[rad/s]
    chasis.structure_base = getReal(pairs["chasis.structure_base"]); //[rad/s]
    chasis.internals[torso] = Game::stringIntoInt(pairs["chasis.internals.torso"]); //[m3]
    chasis.internals[torso_right] = Game::stringIntoInt(pairs["chasis.internals.torso_right"]);
    chasis.internals[torso_left] = Game::stringIntoInt(pairs["chasis.internals.torso_left"]);
    chasis.internals[arm_left] = Game::stringIntoInt(pairs["chasis.internals.arm_left"]);
    chasis.internals[arm_right] = Game::stringIntoInt(pairs["chasis.internals.arm_right"]);
    chasis.internals[leg_left] = Game::stringIntoInt(pairs["chasis.internals.leg_left"]);
    chasis.internals[leg_right] = Game::stringIntoInt(pairs["chasis.internals.leg_right"]);
    chasis.panels[torso] = Game::stringIntoInt(pairs["chasis.panels.torso"]);
    chasis.panels[torso_right] = Game::stringIntoInt(pairs["chasis.panels.torso_right"]);
    chasis.panels[torso_left] = Game::stringIntoInt(pairs["chasis.panels.torso_left"]);
    chasis.panels[arm_right] = Game::stringIntoInt(pairs["chasis.panels.arm_right"]);
    chasis.panels[arm_left] = Game::stringIntoInt(pairs["chasis.panels.arm_left"]);
    chasis.panels[leg_right] = Game::stringIntoInt(pairs["chasis.panels.leg_right"]);
    chasis.panels[leg_left] = Game::stringIntoInt(pairs["chasis.panels.leg_left"]);
    chasis.surface_area[crusader_area::torso] =
        getReal(pairs["chasis.surface_area.torso"]);
    chasis.surface_area[crusader_area::torso_right] =
        getReal(pairs["chasis.surface_area.torso_right"]);
    chasis.surface_area[crusader_area::torso_left] =
        getReal(pairs["chasis.surface_area.torso_left"]);
    chasis.surface_area[crusader_area::arm_left] =
        getReal(pairs["chasis.surface_area.arm_left"]);
    chasis.surface_area[crusader_area::arm_right] =
        getReal(pairs["chasis.surface_area.arm_right"]);
    chasis.surface_area[crusader_area::leg_left] =
        getReal(pairs["chasis.surface_area.leg_left"]);
    chasis.surface_area[crusader_area::leg_right] =
        getReal(pairs["chasis.surface_area.leg_right"]);
    chasis.surface_area[crusader_area::torso_back] =
        getReal(pairs["chasis.surface_area.torso_back"]);
    chasis.surface_area[crusader_area::torso_right_back] =
        getReal(pairs["chasis.surface_area.torso_right_back"]);
    chasis.surface_area[crusader_area::torso_left_back] =
        getReal(pairs["chasis.surface_area.torso_left_back"]);

    return true;
}

/** @brief load definition of the ingame hud
  * fills structs with specs read from a file
  */
bool FilesHandler::getHudDesign(const string& filename, hud_design_t& hud_design)
{
    //prepare map to read data into
    map<string, string> pairs;
    //TODO: replace assert with a fallback or just bail
    assert(getPairs(filename, hud_dir, pairs)); //insert data from file into pairs

    //fill structs with info from pairs
    hud_design.name = filename;
    hud_design.font_name = pairs["hud_design.font_name"];
    getStringArray(hud_design.area_textures, pairs["hud_design.area_textures"]);
    getStringArray(hud_design.area_cover_textures, pairs["hud_design.area_cover_textures"]);
    getEnumPairArray(hud_design.positions, pairs["hud_design.positions"]);
    getIntPairArray(hud_design.sizes, pairs["hud_design.sizes"]);
    getIntPairArray(hud_design.offsets, pairs["hud_design.offsets"]);
    getIntPairArray(hud_design.offsets_alternative, pairs["hud_design.offsets_alternative"]);
    getColourArray(hud_design.status_colours, pairs["hud_design.status_colours"]);
    getColourArray(hud_design.log_colours, pairs["hud_design.log_colours"]);
    getColourArray(hud_design.mfd_colours, pairs["hud_design.mfd_colours"]);
    getColourArray(hud_design.display_colours, pairs["hud_design.display_colours"]);
    getEnumArray(hud_design.mfd_views, pairs["hud_design.mfd_views"]);

    usint i = 0;

    //check to see if a part with a consecutive tag exists
    while (pairs.find(string("hud_design.parts.")+Game::intIntoString(i)) != pairs.end()) {
        //load design of each part
        hud_part_design_t hud_part_design;
        vector<string> hud_part_design_string;
        getStringArray(hud_part_design_string,
                       pairs[string("hud_design.parts.")+Game::intIntoString(i)]);

        //makes sure the are enough values to fill the design
        if (hud_part_design_string.size() < 9) {
            cout << hud_part_design_string.size() << endl;
            return false;
        }

        hud_part_design.area = hud_area(getEnum(hud_part_design_string[0]));
        hud_part_design.name = hud_part_design_string[1];
        hud_part_design.type = hud_part_enum::type(getEnum(hud_part_design_string[2]));
        getRealSeries(hud_part_design.parameters, hud_part_design_string[3]);
        hud_part_design.function = hud_part_enum::function(getEnum(hud_part_design_string[4]));
        hud_part_design.position.first = Game::stringIntoInt(hud_part_design_string[5]);
        hud_part_design.position.second = Game::stringIntoInt(hud_part_design_string[6]);
        hud_part_design.size.first = Game::stringIntoInt(hud_part_design_string[7]);
        hud_part_design.size.second = Game::stringIntoInt(hud_part_design_string[8]);
        hud_design.parts.push_back(hud_part_design);

        ++i;
    }

    //sanity check on values
    if (hud_design.area_textures.size() > 7
            && hud_design.area_cover_textures.size() > 7
            && hud_design.positions.size() > 7
            && hud_design.sizes.size() > 7
            && hud_design.offsets.size() > 7
            && hud_design.offsets_alternative.size() > 7
            && hud_design.status_colours.size() > 0
            && hud_design.log_colours.size() > 0
            && hud_design.mfd_colours.size() > 0
            && hud_design.display_colours.size() > 0) {
        //pad colours if too few
        padHudColours(hud_design.status_colours);
        padHudColours(hud_design.log_colours);
        padHudColours(hud_design.mfd_colours);
        padHudColours(hud_design.display_colours);

    } else {
        cout << "hud definition incomplete" << endl;
        return false;
    }

    return true;
}
