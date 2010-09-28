//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "hud_status_display.h"
#include "status_computer.h"
#include "game.h"
#include "hud.h"

HudStatusDisplay::HudStatusDisplay(hud_part_design_t& a_hud_part_design)
    : HudPart(a_hud_part_design), hud_part_interval(0.1), hud_part_accumulator(0)
{
    //read parameters
    if (a_hud_part_design.parameters.size() < 1) //kill the game if too few params
        Game::kill(string("hud_part missing param: ")+a_hud_part_design.name);
    font_size = a_hud_part_design.parameters[0]; //read the size of the status font

    //line hieght for line poistioning
    usint line_height = (size.second) / hud_num_of_log_lines;

    //create the OGRE text elements to show the log lines
    for (usint i = 0; i < hud_num_of_colours; ++i) { //element for each colour
        for (usint j = 0; j < hud_num_of_status_lines; ++j) { //and each line
            string id = a_hud_part_design.name+"_"+Game::intIntoString(i)+Game::intIntoString(j);
            status_text_elements[i][j]
                = createTextArea(id, "", font_size, Game::hud->hud_design.status_colours[i],
                                 0, (j * line_height), size.first, size.second, container);
        }
    }
}

/** @brief main loop
  */
void HudStatusDisplay::update(Ogre::Real a_dt)
{
    hud_part_accumulator += a_dt; //lower fps for digital parts
    if (hud_part_accumulator > hud_part_interval) { //use accumulator as dt
        //update all lines every time
        for (usint i = 0; i < hud_num_of_colours; ++i) {
            for (usint j = 0; j < hud_num_of_status_lines; ++j) {
                status_text_elements[i][j]->setCaption(Game::hud->status->getLine(i, j));
            }
        }

        hud_part_accumulator = 0;
    }
}


