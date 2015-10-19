// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game_timer.h"
#include "game.h"
#include "game_state.h"

GameTimer::GameTimer(Ogre::Timer* aOgreTimer)
  : OgreTimer(aOgreTimer)
{
  GameTimer::reset();
}

/** @brief slow game to stand still
 */
void GameTimer::pause()
{
  Game::setGameState(game_state_pause);
  TimeWarp = true;
  Rate = 0;
}

/** @brief resume normal time flow
 */
void GameTimer::unpause()
{
  Game::setGameState(game_state_playing);
  TimeWarp = false;
  Rate = 1.0;
}

/** @brief rests both ogre timer and internal timer
 */
void GameTimer::reset()
{
  OgreTimer->reset();

  Start = (ulint)OgreTimer->getMilliseconds();
  Ticks = Start;
}

/** @brief slow down or speed up internal clock
 */
void GameTimer::setRate(Real a_rate)
{
  if (a_rate > 1.0) {
    a_rate = 1.0;
  }

  if (a_rate > 0.99) {
    // normal time flow - disable some of the checks
    TimeWarp = false;
    Rate = 1.0;

  } else {
    // game slowed down or sped up - enable time correction
    TimeWarp = true;
    Rate = a_rate;
  }
}

/** @brief get internal clock detached from real clock
 */
ulint GameTimer::getTicks()
{
  ulint ogre_ticks = (ulint)OgreTimer->getMilliseconds();
  ulint new_ticks = ogre_ticks - Start;

  // get step (new time - old time)
  ulint step = new_ticks - Ticks;

  /*if (TimeWarp && Rate < 0.1) {
    // if the game slows down to a crawl, pause the game
    Game::setGameState(game_state_pause);
    Start += step;
    return 0;
  } else */if (step > 40) {
    // if a frame takes longer than 40ms slow down gameplay to keep a reasonable times slice for sim
    TimeWarp = true;
    Rate = 40.0 / step;
  }

  // keeps the internal time consistent by moving the start time if time is warped or paused
  if (TimeWarp) {
    // recalculate start to accomodate time-warped step
    Start = Start + (ulint)(step * (1 - Rate));

    // measure time against the time warp corrected start
    Ticks = ogre_ticks - Start;

  } else {
    Ticks = new_ticks;
  }

  return Ticks;
}
