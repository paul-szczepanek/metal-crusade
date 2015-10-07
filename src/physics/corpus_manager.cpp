// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "corpus_manager.h"
#include "game_arena.h"
#include "game.h"
#include "corpus.h"
#include "collision_handler.h"

CorpusManager::CorpusManager()
{
}

CorpusManager::~CorpusManager()
{
}

void CorpusManager::update(const Real a_dt)
{
  for (Corpus* c : Corpuses) {
    c->update(a_dt);
  }
}

/** @brief put the dynamic object into the collision system
 */
void CorpusManager::registerStaticObject(Corpus* a_corpus)
{
  Game::Arena->registerObject(a_corpus);
}

/** @brief put the dynamic object into the collision system
 */
void CorpusManager::registerDynamicObject(Corpus* a_corpus)
{
  Game::Arena->registerObject(a_corpus);
  Game::Collision->registerObject(a_corpus);
  Game::Corpus->Corpuses.push_back(a_corpus);
}

/** @brief remove the dynamic object from the collision system
 */
void CorpusManager::deregisterObject(Corpus* a_corpus)
{
  Game::Arena->deregisterObject(a_corpus);
  Game::Collision->deregisterObject(a_corpus);
  Game::Corpus->Corpuses.remove(a_corpus);
}

void CorpusManager::applyForces(const Real a_dt)
{
  Real dts = a_dt * a_dt;
  Vector3 gravity = Vector3(0, 0, -1) * Game::Arena->getGravity() * dts;
  for (Corpus* c : Corpuses) {
    c->Velocity += gravity;
  }
}

void CorpusManager::applyVelocity(const Real a_dt)
{
  for (Corpus* c : Corpuses) {
    c->OldVelocity = c->Velocity;
    c->XYZ += c->Velocity * a_dt;
  }
}
