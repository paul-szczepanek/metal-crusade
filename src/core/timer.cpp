// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "timer.h"
#include "game.h"
#include "game_state.h"

Timer::Timer(Ogre::Timer* aOgreTimer)
  : TimeWarp(false), OgreTimer(aOgreTimer)
{
  Timer::reset();
}

/** @brief slow game to stand still
 */
void Timer::pause()
{
  Game::setGameState(game_state_pause);
  TimeWarp = true;
  Rate = 0;
}

/** @brief resume normal time flow
 */
void Timer::unpause()
{
  Game::setGameState(game_state_playing);
  TimeWarp = false;
  Rate = 1.0;
}

/** @brief rests both ogre timer and internal timer
 */
void Timer::reset()
{
  OgreTimer->reset();

  Start = (ulint)OgreTimer->getMilliseconds();
  Ticks = Start;
}

/** @brief slow down or speed up internal clock
 */
void Timer::setRate(Real aRate)
{
  if (aRate < 1.01 && aRate > 0.99) {
    // normal time flow - disable some of the checks
    TimeWarp = false;
    Rate = 1.0;

  } else {
    // game slowed down or sped up - enable time correction
    TimeWarp = true;
    Rate = aRate;
  }
}

/** @brief get internal clock detached from real clock
 */
ulint Timer::getTicks()
{
  // keeps the internal time consistent by moving the start time if time is warped or paused
  if (TimeWarp) {
    // get step (new time - old time)
    ulint step = (OgreTimer->getMilliseconds() - Start - Ticks);

    // recalculate start to accomodate time-warped step
    Start = Start + (ulint)(step * (1 - Rate));

    // measure time against the time warp corrected start
    Ticks = (ulint)OgreTimer->getMilliseconds() - Start;

  } else {
    // if not warped check if it's not bollocked (mostly for use when debugging, remove later)
    ulint new_ticks = (ulint)OgreTimer->getMilliseconds() - Start;

    if (new_ticks > Ticks + 100) {
      // this basically limits dt to 0.1 seconds by slowing down the game time
      // if fps drops below 10
      Start = Start + (new_ticks - Ticks - 10);

      // measure time again with the corrected start
      Ticks = (ulint)OgreTimer->getMilliseconds() - Start;

    } else {
      Ticks = new_ticks;
    }
  }

  return Ticks;
}
