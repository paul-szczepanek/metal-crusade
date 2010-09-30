//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "ai_factory.h"
#include "unit_ai.h"
#include "crusader_ai.h"

AIFactory::~AIFactory()
{
    //cleanup ais
    for (unsigned int i = 0, for_size = ais.size(); i < for_size; ++i) {
        delete ais[i];
    }
}

/** @brief updates AIs
  * @todo: pretty much everything
  */
void AIFactory::updateAIs()
{
    //call update on every unit
    for (unsigned int i = 0, for_size = ais.size(); i < for_size; ++i) {
        ais[i]->update();
        //TODO give them group plans
    }
}

/** @brief creates ais and add's them to a list
  */
CrusaderAI* AIFactory::createCrusaderAI(Crusader* a_self)
{
    CrusaderAI* ai = new CrusaderAI(a_self);

    //put the unit on the update list
    ais.push_back(ai);

    //TODO put them in groups

    return ai;
}


