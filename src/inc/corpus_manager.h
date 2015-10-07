// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CORPUSMANAGER_H
#define CORPUSMANAGER_H

#include "main.h"

class Corpus;

class CorpusManager
{
public:
  CorpusManager();
  virtual ~CorpusManager();

  void update(Real delta);
  void applyForces(Real delta);
  void applyVelocity(Real delta);

  void registerStaticObject(Corpus* a_corpus);
  void registerDynamicObject(Corpus* a_corpus);
  void deregisterObject(Corpus* a_corpus);

  list<Corpus*> Corpuses;
};

#endif // CORPUSMANAGER_H
