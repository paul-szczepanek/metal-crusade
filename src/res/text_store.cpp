// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "text_store.h"
#include "files_handler.h"
#include "game.h"

static const string GAME_TEXT = "game_text";

TextStore::TextStore()
{
  getGameText(GameText);
}

string& TextStore::getText(string& a_string)
{
  return GameText[getStringKey(a_string)];
}

/** @brief translate string key into int key
 */
size_t TextStore::getStringKey(const string& a_string)
{
  size_t key = 0;

  map<string, size_t>::iterator it = StringMap.find(a_string);

  if(it != StringMap.end()) {
    key = it->second;
  }

  return key;
}

/** @brief fills the vector with game text
 * whilst doing this also creates a map for matching strings to numerical keys used internally
 */
bool TextStore::getGameText(vector<string>& GameText)
{
  string buffer;
  string key; // [key] ignore whatver is after ]
  string value; // whatever is on next line except #comments
  string end_value; // multiline result
  string fs_filename = DATA_DIR + TEXT_DIR + GAME_TEXT;
  ifstream spec_file;
  size_t string_n = 0; // number of keys insrted

  spec_file.open(fs_filename.c_str());

  if(spec_file.is_open()) {
    uint begin;
    uint end;

    while(!spec_file.eof()) {
      // go line by line
      getline(spec_file, buffer);

      // check if it's a key - is it insde []?
      begin = buffer.find_first_of("[");
      end = buffer.find_first_of("]");

      // and check if it's at least one character long
      if(begin < buffer.size() && end < buffer.size() && end - begin > 1) {
        // flush old key and values into vectors if they both exist
        if(!key.empty() && !end_value.empty()) {
          GameText.push_back(end_value);
          StringMap[key] = string_n;
          ++string_n;
        }

        // start collecting new values for new key
        end_value.clear();

        // remove '[' and ']' and save new key
        key = buffer.substr(begin + 1, end - begin - 1);
        FS::stripWhitespace(key);  // remove whitespace if any

      } else {
        // if it's not a key append values to end_value string

        // ignore comments
        end = buffer.find_last_of("#");

        // if # has been found then crop it out
        value = (end < buffer.size()) ? buffer.substr(0, end) : buffer;

        if(!value.empty()) {
          if(end_value.empty()) {   // if there's nothing yetm just copy the value
            end_value = value;

          } else { // there are multiple lines in the string put in newlines between values
            end_value += "\n" + value;
          }
        }
      }
    }

    // last flush after the file ends
    if(!key.empty() && !end_value.empty()) {
      GameText.push_back(end_value);
      StringMap[key] = string_n;
      ++string_n;
    }

    // finished reading file
    spec_file.close();

  } else {
    Game::log(fs_filename + " - missing file, bailing");

    // kill game - no point continuing with missing files
    Game::kill();
  }

  return !GameText.empty();
}
