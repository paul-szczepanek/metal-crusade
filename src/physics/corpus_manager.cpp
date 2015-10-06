// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "corpus_manager.h"
#include "game_arena.h"
#include "game.h"
#include "corpus.h"

CorpusManager::CorpusManager()
{
}

CorpusManager::~CorpusManager()
{
}

void CorpusManager::update(const Real a_dt)
{
  for (auto cell : Game::Arena->LiveCorpusCells) {
    for (auto c : *cell) {
      c->update(a_dt);
    }
  }
}

void CorpusManager::applyForces(const Real a_dt)
{
  Real dts = a_dt * a_dt;
  Vector3 gravity = Vector3(0, 0, -1) * Game::Arena->getGravity() * dts;
  for (auto cell : Game::Arena->LiveCorpusCells) {
    for (auto c : *cell) {
      c->Velocity += gravity;
    }
  }
}

void CorpusManager::applyVelocity(const Real a_dt)
{
  for (auto cell : Game::Arena->LiveCorpusCells) {
    for (auto c : *cell) {
      c->OldVelocity = c->Velocity;
      c->XYZ += c->Velocity * a_dt;
    }
  }
}
