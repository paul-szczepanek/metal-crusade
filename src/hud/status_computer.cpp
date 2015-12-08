// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "status_computer.h"
#include "game.h"

StatusComputer::StatusComputer()
{
  for (size_t i = 0; i < HUD_NUM_OF_COLOURS; ++i) {
    // init char tables for status display
    for (size_t j = 0; j < HUD_NUM_OF_STATUS_LINES; ++j) {
      for (size_t k = 0; k < HUD_STATUS_LINE_LENGTH; ++k) {
        status_lines[i][j][k] = ' ';
      }
      // stick the string terminator in the last char
      status_lines[i][j][HUD_STATUS_LINE_LENGTH] = '\0';
    }
  }
}

/** @brief set status line
 * this takes the line and parses the colours and pads the string to a_length with spaces
 */
void StatusComputer::setLine(const string& a_message,
                             usint         a_line,
                             usint         a_length,
                             usint         a_position)
{
  if (a_position < HUD_STATUS_LINE_LENGTH) {

    // if no length given use string length
    if (a_length == 0) {
      a_length = a_message.length();
    }

    // make sure it doesn't run over the line length
    a_length = min(a_length, static_cast<usint>
                   (HUD_STATUS_LINE_LENGTH - a_position));

    // cap the line number to max available
    if (a_line >= HUD_NUM_OF_STATUS_LINES) {
      a_line = HUD_NUM_OF_STATUS_LINES - 1;
    }

    // fill the lines with monocolour strings
    Game::Hud->parseColours(a_message, a_length, &status_lines[0][a_line][a_position],
                            &status_lines[1][a_line][a_position],
                            &status_lines[2][a_line][a_position]);
  }
}
