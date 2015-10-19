// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

// this it a global include, included by all files

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

#define ABS(a)    ((a) < 0 ? -(a) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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

/** @brief local angle based on an angle passed in
 * converts to a <-pi, pi> range angle in relation to global_angle an angle
 */
inline void localiseAngle(Radian&       angle,
                          const Radian& global_angle)
{
  Radian local_angle;
  Radian local_angle_alt;

  if (global_angle < angle) {
    local_angle = angle - global_angle;
    local_angle_alt = Radian(2 * pi) - (angle - global_angle);
    if (local_angle > local_angle_alt) {
      local_angle = -local_angle_alt;
    }
  } else {
    local_angle = global_angle - angle;
    local_angle_alt = Radian(2 * pi) - (global_angle - angle);
    if (local_angle > local_angle_alt) {
      local_angle = -local_angle_alt;
    }
    local_angle = -local_angle;
  }

  angle = local_angle;
}

// TODO: TEMP put back in hud design

// this is the minimum number of lines hud must implement to be usable
#define HUD_NUM_OF_LOG_LINES          (8)
#define HUD_NUM_OF_STATUS_LINES       (8)
#define HUD_NUM_OF_MFD_AUX_LINES      (5)
// mfd
#define VIEW_TYPES_PER_PAGE    (HUD_NUM_OF_MFD_AUX_LINES - 1) // one line is the label
// text lines max length
#define HUD_LOG_LINE_LENGTH           (60)
#define HUD_STATUS_LINE_LENGTH        (60)
// history size for the log TODO: write excess to file?
#define HUD_MAX_HUD_NUM_OF_LOG_LINES  (40)
#define HUD_MFD_AUX_LINE_LENGTH       (16)
// colour codes for text: regular, exclamation, alternative
// huds can implement them however they want or not at all
#define HUD_NUM_OF_COLOURS            (3) // this is now pretty much hardcoded in parseColours :(
#define HUD_ESCAPE_CHAR               '$' // to get the '$' char in a string type two like so "$$"
#define HUD_REVERT_COLOUR_CHAR        'x' // reverts to the last used colour in the line

const char hud_colour_codes[HUD_NUM_OF_COLOURS] = { 'r', 'e', 'a' };

#endif // MAIN_H_INCLUDED
