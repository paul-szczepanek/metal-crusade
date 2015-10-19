// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef TIMER_H
#define TIMER_H

#include "main.h"

class GameTimer
{
public:
  GameTimer(Ogre::Timer* aOgreTimer);
  ~GameTimer();

  // game time
  ulint getTicks();

  // how fast time flows
  void setRate(Real aRate);
  Real getRate();

  void reset();
  void pause();
  void unpause();

private:
  ulint Start = 0;
  ulint Ticks = 0;

  bool TimeWarp = false;

  // time flow rate
  Real Rate = 0;
  Ogre::Timer* OgreTimer = NULL;
};

inline Real GameTimer::getRate()
{
  return Rate;
}

#endif // TIMER_H
