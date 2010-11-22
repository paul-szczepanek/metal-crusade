//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "imaginarius.h"

ulint Imaginarius::unique_id = 0; //unique id for each imaginary entity in game

/** @brief get a string with a unique id for Ogre
  */
string Imaginarius::getUniqueID()
{
    string id_string;
    stringstream stream;
    stream << setfill('0') << setw(10) << unique_id++; //format id string to 0000000001
    stream >> id_string;
    return id_string;
}

Imaginarius::Imaginarius(Ogre::Vector3 a_pos_xyz) : pos_xyz(a_pos_xyz)
{
    //
}
