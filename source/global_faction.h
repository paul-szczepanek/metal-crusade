//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef GLOBAL_FACTION_H_INCLUDED
#define GLOBAL_FACTION_H_INCLUDED

#include "internal_string.h"

namespace global_faction {

    enum faction {
        mercenary,
        nomads,
        imperium,
        un,
        local,
        faction_max
    };

    const internal_string::string_index faction_names[faction_max] = {
        internal_string::faction_mercenary,
        internal_string::faction_nomads,
        internal_string::faction_imperium,
        internal_string::faction_un
    };
};

#endif // GLOBAL_FACTION_H_INCLUDED
