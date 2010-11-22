//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef HUDPART_H
#define HUDPART_H

#include "main.h"
#include "hud_design.h"

class HudPart
{
public:
    HudPart(hud_part_design_t& a_hud_part_design);
    virtual ~HudPart() { };

    //main loop
    virtual void update(Ogre::Real a_dt) = 0;
    virtual void resize(Ogre::Real a_scale) { }; //for use with 3d elements

protected:
    //basic common properties
    int_pair position;
    int_pair size;
    hud_part_enum::function function;

    //master container for all elements
    Ogre::OverlayContainer* container;

    //short hand for creating elements
    Ogre::OverlayElement* createPanel(const string& a_id, const string& a_material_name,
                                      int a_x, int a_y,  int a_w, int a_h,
                                      Ogre::OverlayContainer* a_container);
    Ogre::OverlayElement* createTextArea(const string& a_id, const string& a_text,
                                         int a_font_size, Ogre::ColourValue a_colour,
                                         int a_x, int a_y,  int a_w, int a_h,
                                         Ogre::OverlayContainer* a_container);

    //unique id for creation
    static string getUniqueID();
    static ulint unique_id;
};

const Ogre::Real hud_3d_distance = -200;

/** @brief creates a unique ID for overlays
  */
inline string HudPart::getUniqueID()
{
    //get unique string from id
    string id_string;
    stringstream stream;
    stream << setfill('0') << setw(4) << ++unique_id; //format id string to 001
    stream >> id_string;
    return id_string;
}

#endif // HUDPART_H
