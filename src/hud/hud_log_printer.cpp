// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_log_printer.h"
#include "log_computer.h"
#include "game.h"

HudLogPrinter::HudLogPrinter(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), printed_log_line(0), timeout(0)
{
  // parameters
  if (a_hud_part_design.parameters.size() < 1) { // kill the game if too few params
    Game::kill(string("hud_part missing param: ") + a_hud_part_design.name);
  }
  font_size = a_hud_part_design.parameters[0]; // read the size of the status font

  // create the paper background material that will scroll with the text
  // first create the material manually because it nneds to wrap
  Ogre::MaterialPtr material = GameHud::createOverlayMaterial(a_hud_part_design.name + "_paper",
                                                          Ogre::TextureUnitState::TAM_WRAP);
  // this material will wrap around whilst scrolling up and down
  paper_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
  paper_texture->setTextureVScale(0.5); // tile it twice

  // now create the element that will use thi material
  string id = a_hud_part_design.name + "_paper";
  createPanel(id, id, -10, 0, size.first + 20, size.second, Container);

  // line height for positioning of the lines
  Real line_height = Real(size.second) / HUD_NUM_OF_STATUS_LINES;

  // create a container for the text elements to guarantee proper z-sorting
  Ogre::OverlayContainer* text_container;
  text_container = static_cast<Ogre::OverlayContainer*>
    (Game::Hud->OgreManager->createOverlayElement("Panel", a_hud_part_design.name + "text_cont"));
  text_container->setMetricsMode(Ogre::GMM_PIXELS);
  Container->addChild(text_container); // add it to the main container so that text is on top

  // create a container for the head and bars to guarantee proper z-sorting
  Ogre::OverlayContainer* head_container;
  head_container = static_cast<Ogre::OverlayContainer*>
    (Game::Hud->OgreManager->createOverlayElement("Panel", a_hud_part_design.name + "head_cont"));
  head_container->setMetricsMode(Ogre::GMM_PIXELS);
  text_container->addChild(head_container); // add it to the text container so it's above the text

  // text for diasplaying log messages
  for (usint i = 0; i < HUD_NUM_OF_COLOURS; ++i) {
    for (usint j = 0; j < HUD_NUM_OF_LOG_LINES; ++j) {
      // create text elements to display the lines - one for each colour
      id = a_hud_part_design.name + "_text_" + intoString(i) + "_" + intoString(j);
      log_text_elements[i][j] = createTextArea(id, "", font_size,
                                               Game::Hud->HudDesign.log_colours[i],
                                               0, (size.second - (j + 1) * line_height),
                                               size.first, line_height, text_container);
    }
  }

  // create the metal bar covering the bottom paper edge
  id = a_hud_part_design.name + "_bar";
  createPanel(id, a_hud_part_design.name + "_bar", -12,
              size.second - 15, size.first + 24, 16, head_container);

  // create the printer head
  id = a_hud_part_design.name + "_head";
  hud_log_head = createPanel(id, a_hud_part_design.name + "_head",
                             -24, size.second - 20, 32, 32, head_container);
}

/** @brief displays the text and moves the printer head
 * TODO: move the paper under the text and the text itself smoothly
 */
void HudLogPrinter::update(Real a_dt)
{
  timeout += a_dt;

  // text for displaying log lines
  if (timeout < line_print_timeout) { // stop checking after the timeout
    for (usint i = 0; i < HUD_NUM_OF_COLOURS; ++i) {
      for (usint j = 0; j < HUD_NUM_OF_LOG_LINES; ++j) {
        // calculate line number
        usint log_line_number = (printed_log_line - j
                                 + HUD_MAX_HUD_NUM_OF_LOG_LINES)
                                % HUD_MAX_HUD_NUM_OF_LOG_LINES;
        // get the log line from hud
        string log_line(Game::Hud->log->getLine(i, log_line_number));

        // if the lines is being currently printed
        if (j == 0 && timeout < line_print_time) {
          // calculate the head position
          usint head_position = (timeout / line_print_time)
                                * HUD_LOG_LINE_LENGTH;

          // only print as much of the string as appears to the left of the head
          log_text_elements[i][j]->setCaption(log_line.substr(0, head_position));

        } else {
          // otherwise set the whole line
          log_text_elements[i][j]->setCaption(log_line);
        }
      }
    }
  }

  // moving the printer head
  if (timeout < line_print_time) {
    // start moving head right
    hud_log_head->setPosition(-24 + (timeout / line_print_time) * (size.first + 20),
                              size.second - 20);

  } else if (timeout < line_print_timeout) {
    // printing finished start returning the head left
    Real returned = (timeout - line_print_time) / (line_print_timeout - line_print_time);
    hud_log_head->setPosition(-24 + (1 - returned) * (size.first + 20), size.second - 20);

  } else if (printed_log_line != Game::Hud->log->getCurrentLine()) { // new line is available
    // advance line if there is feed
    ++printed_log_line;
    timeout = 0;

    // move paper
    Real current_v_scroll = paper_texture->getTextureVScroll();
    paper_texture->setTextureScroll(0, current_v_scroll
                                    - 1 / HUD_NUM_OF_STATUS_LINES);

  } else {
    // if nothing to do stay at rest
    hud_log_head->setPosition(-24, position.second + size.second - 20);
  }
}
