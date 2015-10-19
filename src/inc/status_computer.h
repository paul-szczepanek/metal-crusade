// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef STATUSCOMPUTER_H
#define STATUSCOMPUTER_H

#include "main.h"
#include "game_hud.h"
#include "hud_design.h"

class StatusComputer
{
public:
  StatusComputer();
  ~StatusComputer()
  {
  }

  // communicate with the status line
  void setLine(const string& a_message,
               usint         a_line,
               usint         a_length = 0,
               usint         a_position = 0);
  string getLine(usint a_i,
                 usint a_j)
  {
    return status_lines[a_i][a_j];
  }

private:
  char status_lines[HUD_NUM_OF_COLOURS][HUD_NUM_OF_STATUS_LINES][HUD_STATUS_LINE_LENGTH + 1];
};

#endif // STATUSCOMPUTER_H
