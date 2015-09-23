// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef TIMER_H
#define TIMER_H

#include "main.h"

class Timer
{
public:
  Timer(Ogre::Timer* aOgreTimer);
  ~Timer() { };

  // game time
  long lint getTicks();

  // how fast time flows
  void setRate(Ogre::Real aRate);
  Ogre::Real getRate() { return Rate; };

  void reset();
  void pause();
  void unpause();

private:
  long lint Start;
  long lint Ticks;

  bool TimeWarp;

  // time flow rate
  Ogre::Real Rate;
  Ogre::Timer* OgreTimer;
};

#endif // TIMER_H
