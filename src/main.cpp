// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "main.h"
#include "game.h"

#ifdef PLATFORM_WIN32
int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            char*,
            int       nShowCmd)
#else
int main(int    argc,
         char** argv)
#endif
{
  bool debug_on = true;
  for (size_t i = 1; i < argc; ++i) {
    ++argv;
    if (strcmp("-debug", *argv) == 0) {
      debug_on == true;
    } else if (strcmp("-d", *argv) == 0) {
      debug_on == true;
    }
  }

  Game::init(debug_on);

  Game::instance()->run();

  delete Game::instance();

  return 0;
}
