//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef TIMER_H
#define TIMER_H

#include "main.h"

class Timer
{
public:
    Timer(Ogre::Timer* a_ogre_timer);
    ~Timer() { };

    //game time
    long long int getTicks();

    //how fast time flows
    void setRate(Ogre::Real a_rate);
    Ogre::Real getRate() { return rate; };

    void reset();
    void pause();
    void unpause();

private:
    long long int start;
    long long int ticks;

    bool time_warp;

    //time flow rate
    Ogre::Real rate;
    Ogre::Timer* ogre_timer;
};

#endif // TIMER_H
