//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef AI_FACTORY_H
#define AI_FACTORY_H

#include "main.h"

class UnitAI;
class CrusaderAI;
class Crusader;
class TankAI;
class Tank;

class AIFactory
{
public:
    AIFactory() { };
    ~AIFactory();

    //main loop
    void update();

    //ai creation
    CrusaderAI* createCrusaderAI(Crusader* a_self);
    TankAI* createCrusaderAI(Tank* a_self);

private:
    vector<UnitAI*> ais;
};

#endif // AI_FACTORY_H
