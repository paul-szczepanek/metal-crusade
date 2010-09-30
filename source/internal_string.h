//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef INTERNAL_STRING_H_INCLUDED
#define INTERNAL_STRING_H_INCLUDED

namespace internal_string {
    enum string_index {
        missing_string,
        select_view_type, //mfd
        damage_diagram_self,
        damage_diagram_target,
        damage_view_target,
        inspector,
        minimap,
        enemy_list,
        systems_control,
        squad_tactic,
        shock_damage_sustained, //log messages
        faction_mercenary, //faction names
        faction_nomads,
        faction_imperium,
        faction_un,
    };
};

#endif // INTERNAL_STRING_H_INCLUDED
