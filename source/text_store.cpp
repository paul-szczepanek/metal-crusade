//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "text_store.h"
#include "files_handler.h"

TextStore::TextStore()
{
    FilesHandler::getGameText(game_text);
}

string& TextStore::getText(string& a_string)
{
    return game_text[FilesHandler::getStringKey(a_string)];
}
