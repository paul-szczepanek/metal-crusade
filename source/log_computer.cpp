//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "log_computer.h"
#include "hud.h"
#include "game.h"

LogComputer::LogComputer() : current_log_line(0)
{
    for (usint i = 0; i < hud_num_of_colours; ++i) {
        //init char tables for log lines
        for (usint j = 0; j < hud_max_hud_num_of_log_lines; ++j) {
            for (usint k = 0; k < hud_log_line_length; ++k) {
                log_lines[i][j][k] = ' ';
            }
            //stick the string terminator in the last char
            log_lines[i][j][hud_log_line_length] = '\0';
        }
    }
}

/** @brief adds a log message - parses the colours and pads with spaces to max length
  */
void LogComputer::addLine(const string& a_message)
{
    //advance log
    usint log_line_number = (++current_log_line) % hud_max_hud_num_of_log_lines;

    Game::hud->parseColours(a_message, hud_log_line_length, log_lines[0][log_line_number],
                            log_lines[1][log_line_number], log_lines[2][log_line_number]);
}

/** @brief overwrites last message in the log
  */
void LogComputer::changeLine(const string& a_message)
{
    //keep the log on the same line
    usint log_line_number = current_log_line % hud_max_hud_num_of_log_lines;

    Game::hud->parseColours(a_message, hud_log_line_length, log_lines[0][log_line_number],
                            log_lines[1][log_line_number], log_lines[2][log_line_number]);
}
