//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef LOGCOMPUTER_H
#define LOGCOMPUTER_H

#include "main.h"
#include "hud_design.h"

class LogComputer
{
public:
    LogComputer();
    ~LogComputer() { };

    //communicating with log
    void addLine(const string& a_message);
    void changeLine(const string& a_message);
    string getLine(usint a_i, usint a_j) { return log_lines[a_i][a_j]; };

    unsigned int getCurrentLine() { return current_log_line; };

private:
    char log_lines[hud_num_of_colours][hud_max_hud_num_of_log_lines][hud_log_line_length + 1];

    //tracks the position of what is considered the last log line and wraps around the array
    unsigned int current_log_line;
};

#endif // LOGCOMPUTER_H
