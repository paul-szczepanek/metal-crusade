//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "main.h"
#include "game.h"

int main (int argc, char** argv)
{
    Game::instantiate();

    Game::instance()->init();

    delete Game::instance();

    return 0;
}
