// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef TIMER_H
#define TIMER_H

#include "main.h"

class Timer
{
public:
  Timer(Ogre::Timer* aOgreTimer);
  ~Timer();

  // game time
  ulint getTicks();

  // how fast time flows
  void setRate(Real aRate);
  Real getRate()
  {
    return Rate;
  }

  void reset();
  void pause();
  void unpause();

private:
  ulint Start;
  ulint Ticks;

  bool TimeWarp;

  // time flow rate
  Real Rate;
  Ogre::Timer* OgreTimer;
};

#endif // TIMER_H
