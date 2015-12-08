// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "log_computer.h"
#include "game.h"

LogComputer::LogComputer()
  : current_log_line(0)
{
  for (size_t i = 0; i < HUD_NUM_OF_COLOURS; ++i) {
    // init char tables for log lines
    for (size_t j = 0; j < HUD_MAX_HUD_NUM_OF_LOG_LINES; ++j) {
      for (size_t k = 0; k < HUD_LOG_LINE_LENGTH; ++k) {
        log_lines[i][j][k] = ' ';
      }
      // stick the string terminator in the last char
      log_lines[i][j][HUD_LOG_LINE_LENGTH] = '\0';
    }
  }
}

/** @brief adds a log message - parses the colours and pads with spaces to max length
 */
void LogComputer::addLine(const string& a_message)
{
  // advance log
  usint log_line_number = (++current_log_line) % HUD_MAX_HUD_NUM_OF_LOG_LINES;

  Game::Hud->parseColours(a_message, HUD_LOG_LINE_LENGTH, log_lines[0][log_line_number],
                          log_lines[1][log_line_number], log_lines[2][log_line_number]);
}

/** @brief overwrites last message in the log
 */
void LogComputer::changeLine(const string& a_message)
{
  // keep the log on the same line
  usint log_line_number = current_log_line % HUD_MAX_HUD_NUM_OF_LOG_LINES;

  Game::Hud->parseColours(a_message, HUD_LOG_LINE_LENGTH, log_lines[0][log_line_number],
                          log_lines[1][log_line_number], log_lines[2][log_line_number]);
}
