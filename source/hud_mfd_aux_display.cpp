//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "hud_mfd_aux_display.h"
#include "mfd_computer.h"
#include "game.h"
#include "hud.h"

HudMFDAuxDisplay::HudMFDAuxDisplay(hud_part_design_t& a_hud_part_design)
    : HudPart(a_hud_part_design), hud_part_interval(0.1), hud_part_accumulator(0)
{
    //read parameters
    if (a_hud_part_design.parameters.size() < 1) //kill the game if too few params
        Game::kill(string("hud_part missing param: ")+a_hud_part_design.name);
    font_size = a_hud_part_design.parameters[0]; //read the size of the mfd aux font

    //line height for line positioning
    usint line_height = (size.second) / hud_num_of_mfd_aux_lines;

    //create the OGRE text elements to show the mfd aux lines
    for (usint i = 0; i < hud_num_of_colours; ++i) { //element for each colour
        for (usint j = 0; j < hud_num_of_mfd_aux_lines; ++j) { //and each line
            string id = a_hud_part_design.name+"_"+Game::intIntoString(i)+Game::intIntoString(j);
            mfd_aux_text_elements[i][j]
                = createTextArea(id, "", font_size, Game::hud->hud_design.mfd_colours[i],
                                 0, (j * line_height), size.first, size.second, container);
        }
    }

    //get the mfd to hook up to
    mfd = Game::hud->getMFD();
}

void HudMFDAuxDisplay::update(Ogre::Real a_dt)
{
    hud_part_accumulator += a_dt; //lower fps for digital parts
    if (hud_part_accumulator > hud_part_interval) { //use accumulator as dt
        //update all lines every time
        for (usint i = 0; i < hud_num_of_colours; ++i) {
            for (usint j = 0; j < hud_num_of_mfd_aux_lines; ++j) {
                mfd_aux_text_elements[i][j]->setCaption(mfd->getLine(i, j));
            }
        }

        hud_part_accumulator = 0;
    }
}


