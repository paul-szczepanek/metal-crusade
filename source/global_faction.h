//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef GLOBAL_FACTION_H_INCLUDED
#define GLOBAL_FACTION_H_INCLUDED

#include "internal_string.h"

namespace global_faction {

    //global factions are archetypes and need to be part of the code because they behave differently
    //and run different code
    //they can have several top factions that are parameterising the code for each of these
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
        internal_string::faction_un,
        internal_string::faction_local
    };
};

#endif // GLOBAL_FACTION_H_INCLUDED
