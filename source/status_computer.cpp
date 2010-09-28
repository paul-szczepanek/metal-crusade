//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "status_computer.h"
#include "hud.h"
#include "game.h"

StatusComputer::StatusComputer()
{
    for (usint i = 0; i < hud_num_of_colours; ++i) {
        //init char tables for status display
        for (usint j = 0; j < hud_num_of_status_lines; ++j) {
            for (usint k = 0; k < hud_status_line_length; ++k) {
                status_lines[i][j][k] = ' ';
            }
            //stick the string terminator in the last char
            status_lines[i][j][hud_status_line_length] = '\0';
        }
    }
}

/** @brief set status line
  * this takes the line and parses the colours and pads the string to a_length with spaces
  */
void StatusComputer::setLine(const string& a_message, usint a_line,
                        usint a_length, usint a_position)
{
    if (a_position < hud_status_line_length) {

        //if no length given use string length
        if (a_length == 0) {
            a_length = a_message.length();
        }

        //make sure it doesn't run over the line length
        a_length = min(a_length, static_cast<usint>
                            (hud_status_line_length - a_position));

        //cap the line number to max available
        if (a_line >= hud_num_of_status_lines) {
            a_line = hud_num_of_status_lines - 1;
        }

        //fill the lines with monocolour strings
        Game::hud->parseColours(a_message, a_length, &status_lines[0][a_line][a_position],
                                &status_lines[1][a_line][a_position],
                                &status_lines[2][a_line][a_position]);
    }
}
