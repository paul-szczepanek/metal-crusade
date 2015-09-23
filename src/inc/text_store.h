// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef TEXTSTORE_H
#define TEXTSTORE_H

#include "main.h"
#include "internal_string.h"

// just the barebones for now

class TextStore
{
public:
  TextStore();
  ~TextStore();

  // gets the string based on a string key - slow, used in files
  string& getText(string& a_string);

  // internally string ids are stored as ints and these return the strings based on such keys
  string& getText(internal_string::string_index a_string_key);
  string& getText(size_t a_string_key);

  // game strings
  bool getGameText(vector<string>& GameText);
  size_t getStringKey(const string& a_string);

  // stores game string keys used in files
  map<string, size_t> StringMap;

private:
  // stores all the text in game, indexed by the size_t string key
  vector<string> GameText;
};

inline string& TextStore::getText(internal_string::string_index a_string_key)
{
  return GameText[a_string_key];
}

inline string& TextStore::getText(size_t a_string_key)
{
  return GameText[a_string_key];
}

#endif // TEXTSTORE_H
