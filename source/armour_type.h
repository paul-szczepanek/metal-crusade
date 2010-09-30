//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef ARMOUR_TYPE_H
#define ARMOUR_TYPE_H

enum armour_type {
    armour_type_composite,
    armour_type_reactive,
    armour_type_plastic,
    armour_type_ceramic,
    armour_type_tesla
};

//TODO: read from file
const Ogre::Real armour_type_to_structure[] = {
    1,   //armour_type_composite
    0.25,//armour_type_reactive
    2,   //armour_type_plastic
    1,   //armour_type_ceramic
    4,   //armour_type_tesla
};

const Ogre::Real armour_type_to_ballistic[] = {
    1,   //armour_type_composite
    8,   //armour_type_reactive
    1,   //armour_type_plastic
    0.5, //armour_type_ceramic
    4,   //armour_type_tesla
};

const Ogre::Real armour_type_to_conductivity[] = {
    0.1, //armour_type_composite
    0.4, //armour_type_reactive
    0.8, //armour_type_plastic
    1,   //armour_type_ceramic
    0.5, //armour_type_tesla
};

const Ogre::Real armour_type_to_generated_heat[] = {
    0,   //armour_type_composite
    0.5, //armour_type_reactive
    0,   //armour_type_plastic
    0,   //armour_type_ceramic
    2,   //armour_type_tesla
};

#endif // ARMOUR_TYPE_H
