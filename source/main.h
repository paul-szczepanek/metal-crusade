//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef PLATFORM_WIN32
    #include <Ogre.h>
#else
    #include <OGRE/Ogre.h>
#endif


//this it a global include inlcuded by all files

const Ogre::Real pi         = 3.14159265358979323846;
const Ogre::Real root_of_2  = 1.41421356237309504880;
const Ogre::Real c1o6       = 0.16666666666666666667;
const Ogre::Real c1o3       = 0.33333333333333333333;
const Ogre::Real c2o3       = 0.66666666666666666667;
const Ogre::Real c2o9       = 0.22222222222222222222;
const Ogre::Real c4o9       = 0.44444444444444444444;
const Ogre::Real degree2rad = 0.01745329251994329576;

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

typedef pair<int, int> int_pair;
typedef pair<unsigned int, unsigned int> uint_pair;
typedef pair<unsigned short int, unsigned short int> usint_pair;
typedef pair<unsigned long int, unsigned long int> ulint_pair;
typedef pair<Ogre::Real, Ogre::Real> real_pair;
typedef unsigned short int usint;
typedef unsigned long int ulint;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef Ogre::TextureUnitState::TextureAddressingMode texture_addressing;

template <typename T> inline void clamp(T& value, const T& min, const T& max)
{
  value = value < min? min : (value > max? max : value);
}

template <typename T> inline void clampZero(T& value, const T& max)
{
  value = value < 0? 0 : (value > max? max : value);
}

template <typename T> inline void clampZeroOne(T& value)
{
  value = value < 0? 0 : (value > 1? 1 : value);
}

template <typename T> inline string intoString(const T& a_thing)
{
    stringstream stream;
    stream << a_thing;
    return stream.str();
}

template <typename T> inline Ogre::Real intoReal(const T& a_thing)
{
    stringstream stream;
    Ogre::Real real;
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

inline string realIntoString(const Ogre::Real a_real, const usint a_precision = 3)
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

#endif // MAIN_H_INCLUDED
