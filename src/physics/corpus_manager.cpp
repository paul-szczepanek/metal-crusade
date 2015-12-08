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
  applyForces(a_dt);

  Game::Collision->update(a_dt);

  applyVelocity(a_dt);

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
  Corpuses.push_back(a_corpus);
}

/** @brief remove the dynamic object from the collision system
 */
void CorpusManager::deregisterObject(Corpus* a_corpus)
{
  Game::Arena->deregisterObject(a_corpus);
  Game::Collision->deregisterObject(a_corpus);
  Corpuses.remove(a_corpus);
}

#define TERM_VELOCITY ((Real)100)
#define TERM_VELOCITY_CONSTANT (Real)(1 / ((TERM_VELOCITY) *(TERM_VELOCITY)))

void CorpusManager::applyForces(const Real a_dt)
{
  Vector3 gravity = Vector3(0, -1, 0) * Game::Arena->getGravity() * a_dt;
  for (Corpus* c : Corpuses) {
    c->Velocity += gravity;
    // retard motion while the physics weep
    c->Velocity *= 1 - min(c->Velocity.squaredLength() * TERM_VELOCITY_CONSTANT * a_dt, Real(0.9));
  }
}

void CorpusManager::applyVelocity(const Real a_dt)
{
  for (Corpus* c : Corpuses) {
    c->OldVelocity = c->Velocity;
    c->move(c->Velocity * a_dt);
  }
}
