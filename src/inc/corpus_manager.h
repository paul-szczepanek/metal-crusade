// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef CORPUSMANAGER_H
#define CORPUSMANAGER_H

#include "main.h"

class CorpusManager
{
public:
  CorpusManager();
  virtual ~CorpusManager();

  void update(Real delta);
  void applyForces(Real delta);
  void applyVelocity(Real delta);

private:

};

#endif // CORPUSMANAGER_H
