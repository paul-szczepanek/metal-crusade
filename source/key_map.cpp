//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "key_map.h"
#include "files_handler.h"

/** @brief sets the mappings
  */
KeyMap::KeyMap()
{
    assert(FilesHandler::getKeyConfig("key_config", keys, buttons));
}
