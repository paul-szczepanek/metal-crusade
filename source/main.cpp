//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "main.h"
#include "game.h"

#ifdef PLATFORM_WIN32
    int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
#else
    int main (int argc, char** argv)
#endif
{
    Game::instantiate();

    Game::instance()->init();

    delete Game::instance();

    return 0;
}
