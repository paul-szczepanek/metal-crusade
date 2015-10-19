// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef LOGCOMPUTER_H
#define LOGCOMPUTER_H

#include "main.h"
#include "game_hud.h"
#include "hud_design.h"

class LogComputer
{
public:
  LogComputer();
  ~LogComputer()
  {
  }

  // communicating with log
  void addLine(const string& a_message);
  void changeLine(const string& a_message);
  string getLine(usint a_i,
                 usint a_j)
  {
    return log_lines[a_i][a_j];
  }

  size_t getCurrentLine()
  {
    return current_log_line;
  }

private:
  char log_lines[HUD_NUM_OF_COLOURS][HUD_MAX_HUD_NUM_OF_LOG_LINES][HUD_LOG_LINE_LENGTH + 1];

  // tracks the position of what is considered the last log line and wraps around the array
  size_t current_log_line;
};

#endif // LOGCOMPUTER_H
