// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef PAIR_EQUALS_H
#define PAIR_EQUALS_H

#include "main.h"

template <class T1, class T2> struct PairFirstEquals {
  PairFirstEquals (const T1& a_value) : value_first(a_value) { };

  // the value being looked for
  T1 value_first;

  // comparison for stl find_if
  bool operator() (pair<T1, T2> a_value) { return value_first == a_value.first; }
};

template <class T1, class T2> struct PairSecondEquals {
  PairSecondEquals (const T2& a_value) : value_second(a_value) { };

  // the value being looked for
  T2 value_second;

  // comparison for stl find_if
  bool operator() (pair<T1, T2> a_value) { return value_second == a_value.second; }
};

#endif // PAIR_EQUALS_H
