//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "timer.h"
#include "game.h"

Timer::Timer(Ogre::Timer* a_ogre_timer) : time_warp(false), ogre_timer(a_ogre_timer)
{
    Timer::reset();
};

/** @brief slow game to stand still
  */
void Timer::pause()
{
    time_warp = true;
    rate = 0;
}

/** @brief resume normal time flow
  */
void Timer::unpause()
{
    time_warp = false;
    rate = 1.0;
}

/** @brief rests both ogre timer and internal timer
  */
void Timer::reset()
{
    ogre_timer->reset();

    start = (long long int)ogre_timer->getMilliseconds();
    ticks = start;
}

/** @brief slow down or speed up internal clock
  */
void Timer::setRate(float a_rate)
{

    if (a_rate == 1.0) {
        //normal time flow - disable some of the checks
        time_warp = false;
        rate = 1.0;

    } else {
        //game slowed down or sped up - enable time correction
        time_warp = true;
        rate = a_rate;
    }
}

/** @brief get internal clock detached from real clock
  */
long long int Timer::getTicks()
{
    //keeps the internal time consistent by moving the start time if time is warped or paused
    if (time_warp) {
        //get step (new time - old time)
        int step = (ogre_timer->getMilliseconds() - start - ticks);

        //recalculate start to accomodate time-warped step
        start = start + (long long int)(step * (1 - rate));

        //measure time against the time warp corrected start
        ticks = (long long int)ogre_timer->getMilliseconds() - start;

    } else {
        //if not warped check if it's not bollocked (mostly for use when debugging, remove later)
        long long int new_ticks = (long long int)ogre_timer->getMilliseconds() - start;

        if (new_ticks > ticks + 100) {
            //this basically limits dt to 0.1 seconds by slowing down the game time
            //if fps drops below 10
            start = start + (new_ticks - ticks - 10);

            //measure time again with the corrected start
            ticks = (long long int)ogre_timer->getMilliseconds() - start;

        } else {
            ticks = new_ticks;
        }
    }

    return ticks;
}

