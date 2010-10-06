//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <OGRE/Ogre.h>

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

#endif // MAIN_H_INCLUDED
