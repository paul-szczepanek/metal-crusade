// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

#ifdef PLATFORM_WIN32
#include <Ogre.h>
#else
#include <OGRE/Ogre.h>
#endif

// this it a global include included by all files

using Ogre::Real;
using Ogre::Quaternion;
using Ogre::Vector2;
using Ogre::Vector3;
using Ogre::Radian;

const Real pi         = 3.14159265358979323846;
const Real root_of_2  = 1.41421356237309504880;
const Real c1o6       = 0.16666666666666666667;
const Real c1o3       = 0.33333333333333333333;
const Real c2o3       = 0.66666666666666666667;
const Real c2o9       = 0.22222222222222222222;
const Real c4o9       = 0.44444444444444444444;
const Real degree2rad = 0.01745329251994329576;

using std::cout;
using std::max;
using std::min;
using std::endl;
using std::pair;
using std::count;
using std::acos;
using std::cos;
using std::sin;
using std::asin;
using std::deque;
using std::log;
using std::sort;
using std::setiosflags;
using std::sqrt;
using std::log10;
using std::iterator;
using std::vector;
using std::set;
using std::list;
using std::string;
using std::stringstream;
using std::abs;
using std::setprecision;
using std::setw;
using std::setfill;
using std::bitset;
using std::map;
using std::multimap;
using std::ofstream;
using std::ifstream;
using std::make_pair;
using std::find;
using std::find_if;

typedef int16_t sint;
typedef int64_t lint;
typedef uint16_t usint;
typedef uint64_t ulint;
typedef unsigned char uchar;

typedef pair<int, int> int_pair;
typedef pair<size_t, size_t> size_t_pair;
typedef pair<usint, usint> usint_pair;
typedef pair<ulint, ulint> ulint_pair;
typedef pair<Real, Real> real_pair;

typedef Ogre::TextureUnitState::TextureAddressingMode texture_addressing;

template <typename T> inline void clamp(T&       value,
                                        const T& min,
                                        const T& max)
{
  value = value < min ? min : (value > max ? max : value);
}

template <typename T> inline void clampZero(T&       value,
                                            const T& max)
{
  value = value < 0 ? 0 : (value > max ? max : value);
}

template <typename T> inline void clampZeroOne(T& value)
{
  value = value < 0 ? 0 : (value > 1 ? 1 : value);
}

template <typename T> inline string intoString(const T& a_thing)
{
  stringstream stream;
  stream << a_thing;
  return stream.str();
}

template <typename T> inline Real intoReal(const T& a_thing)
{
  stringstream stream;
  Real real;
  stream << a_thing;
  stream >> real;
  return real;
}

template <typename T> inline int intoInt(const T& a_thing)
{
  stringstream stream;
  int integer;
  stream << a_thing;
  stream >> integer;
  return integer;
}

inline string realIntoString(const Real  a_real,
                             const usint a_precision = 3)
{
  stringstream stream;
  stream << setprecision(a_precision) << a_real;
  return stream.str();
}

inline bool take(bool& do_once)
{
  if (do_once) {
    do_once = false;
    return true;
  }
  return false;
}

// TODO: TEMP put back in hud design

// this is the minimum number of lines hud must implement to be usable
const usint HUD_NUM_OF_LOG_LINES = 8;
const usint hud_num_of_status_lines = 8;
const usint hud_num_of_mfd_aux_lines = 5;
// mfd
const usint view_types_per_page = hud_num_of_mfd_aux_lines - 1; // one line is the label
// text lines max length
const usint HUD_LOG_LINE_LENGTH = 60;
const usint hud_status_line_length = 60;
// history size for the log TODO: write excess to file?
const usint hud_max_HUD_NUM_OF_LOG_LINES = 40;
const usint hud_mfd_aux_line_length = 16;
// colour codes for text: regular, exclamation, alternative
// huds can implement them however they want or not at all
const usint hud_num_of_colours = 3; // this is now pretty much hardcoded in parseColours :(
const char hud_colour_codes[hud_num_of_colours] = { 'r', 'e', 'a' };
const char hud_escape_char = '$'; // to get the '$' char in a string type two like so "$$"
const char hud_revert_colour_char = 'x'; // reverts to the last used colour in the line

#endif // MAIN_H_INCLUDED
