//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef CRUSADERAI_H
#define CRUSADERAI_H

class Crusader;
class Corpus;

#include "unit_ai.h"

class CrusaderAI : public UnitAI
{
public:
    CrusaderAI(Crusader* a_self);
    virtual ~CrusaderAI() { };

    //main loop
    void update();

private:
    Corpus* enemy;
    Crusader* self;
};

#endif // CRUSADERAI_H
