//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef TEXTSTORE_H
#define TEXTSTORE_H

#include "main.h"
#include "internal_string.h"

//just the barebones for now

class TextStore
{
public:
    TextStore();
    ~TextStore();

    //gets the string based on a string key - slow, used in files
    string& getText(string& a_string);

    //internally string ids are stored as ints and these return the strings based on such keys
    string& getText(internal_string::string_index a_string_key);
    string& getText(ulint a_string_key);

private:
    //stores all the test in game, indexed by the ulint string key
    vector<string> game_text;
};

inline string& TextStore::getText(internal_string::string_index a_string_key)
{
    return game_text[a_string_key];
}

inline string& TextStore::getText(ulint a_string_key)
{
    return game_text[a_string_key];
}

#endif // TEXTSTORE_H
