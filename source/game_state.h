//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef GAMESTATE_H
#define GAMESTATE_H

enum game_state {
    game_state_init,
    game_state_menu,
    game_state_loading,
    game_state_playing,
    game_state_pause,
    game_state_global_map,
    game_state_closing
};

#endif // GAMESTATE_H
