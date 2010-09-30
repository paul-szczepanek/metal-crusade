//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef MFD_VIEW_DESIGN_H_INCLUDED
#define MFD_VIEW_DESIGN_H_INCLUDED

#include "internal_string.h"

//multi function displays' different views
namespace mfd_view {
    enum view_type {
        damage_diagram_self,
        damage_diagram_target,
        damage_view_target,
        inspector,
        minimap,
        enemy_list,
        systems_control,
        squad_tactic
    };

    const internal_string::string_index view_names[] = {
        internal_string::damage_diagram_self,
        internal_string::damage_diagram_target,
        internal_string::damage_view_target,
        internal_string::inspector,
        internal_string::minimap,
        internal_string::enemy_list,
        internal_string::systems_control,
        internal_string::squad_tactic
    };

    const usint num_of_diagram_types = 3;

    enum diagram_type {
        blank,
        object,
        biped_crusader
    };

    const usint num_of_diagram_elements[3] = {
        0,
        1,
        10
    };
};

#endif // MFD_VIEW_DESIGN_H_INCLUDED
