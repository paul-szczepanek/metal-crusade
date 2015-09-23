// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef POLICY_H
#define POLICY_H

#include "main.h"
#include "global_faction.h"

namespace policy
{
enum response {
  protect, // actively come to aid, die trying to protect you
  give_help, // attack your enemy if in sight, self-preservation intact
  defend_yourself, // only defend against direct attack, a unit will defend itself
  attack_if_attacked, // neutral, will attack your group if any unit gets attacked
  attack_on_sight, // will attack if you get near
  seek_and_destroy, // will chase you down
  response_max
}

struct rules {
  Real centralisation; // (0,1) how relations are propagated (0 ignores parents)

  Real aggression; // (0,1) how easy it is to anger (0 ignores you)
  Real protection; // (0,1) how much it cares for child factions (0 no help for children)
  Real grudge; // (0,) how quickly it forgets aggressions, pert turn (0 means instant)
  Real loyalty; // (0,) how quickly good relations deteriorate, per turn (0 - instant)
}

rules defaults[global_faction::faction_max] {
  // mercenary,
  { 0,    1,    0,     10,     10 },
  // nomads,
  { 0.25, 1,    0.25,  1000,   10 },
  // imperium,
  { 0.75, 0.5,  0.5,   100,    100 },
  // un,
  { 1,    0.25, 1,     10,     1000 },
  // local,
  { 1,    1,    1,     10,     1000 },
}

}

#endif // POLICY_H
