// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "hud_weapon_tab.h"
#include "game.h"
#include "unit.h"
#include "weapon.h"
#include "hud.h"
#include "text_store.h"
#include "game_controller.h"

HudWeaponTab::HudWeaponTab(hud_part_design_t& a_hud_part_design)
  : HudPart(a_hud_part_design), interval(0.1), accumulator(0)
{
  Ogre::OverlayManager* overlay_mngr = Ogre::OverlayManager::getSingletonPtr();

  // alias the vector of weapons for easy access here
  weapons = Game::hud->player_unit->getWeapons();

  // resize the container to accomodate the number of weapons
  container->setDimensions(size.first, size.second * weapons.size());

  // get font size
  if (a_hud_part_design.parameters.size() < 1) { // kill the game if too few params
    Game::kill(string("hud_part missing param: ") + a_hud_part_design.name);
  }
  font_size = a_hud_part_design.parameters[0]; // read the size of the status font

  // numbers for positioning the lights
  Ogre::Real offset = Ogre::Real(size.first) * 0.3;
  Ogre::Real step = (Ogre::Real(size.first - offset) * 0.94) / number_of_lights;
  Ogre::Real width = step * 2.8;

  // create a container for the lights to guarantee proper z-sorting
  Ogre::OverlayContainer* lights_container = static_cast<Ogre::OverlayContainer*>
      (overlay_mngr->createOverlayElement("Panel",
                                          a_hud_part_design.name + "lights_container"));
  lights_container->setMetricsMode(Ogre::GMM_PIXELS);
  container->addChild(lights_container); // add it to the main container

  // create a container for the text elements to guarantee proper z-sorting
  Ogre::OverlayContainer* text_container = static_cast<Ogre::OverlayContainer*>
      (overlay_mngr->createOverlayElement("Panel",
                                          a_hud_part_design.name + "text_container"));
  text_container->setMetricsMode(Ogre::GMM_PIXELS);
  lights_container->addChild(text_container); // add it to the lights container so text is on top

  // for each weapon create a tab with lights and text and ammo
  for (usint i = 0, for_size = weapons.size(); i < for_size; ++i) {
    // create the baground tab
    string id = a_hud_part_design.name + "_tab_bg_" + intoString(i);
    createPanel(id, a_hud_part_design.name,
                0, size.second * i, size.first, size.second, container);

    // create lights showig timout progress
    weapon_tab_lights_t lights;
    for (usint j = 0; j < number_of_lights; ++j) {
      // create element for each light colour
      // green
      id = a_hud_part_design.name + "_grn_" + intoString(i) + "_" + intoString(j);
      lights.green[j] = createPanel(id, a_hud_part_design.name + "_green",
                                    offset + step * (0.5 + j) - width / 2, size.second * i,
                                    width, size.second, lights_container);
      // yellow
      id = a_hud_part_design.name + "_ylw_" + intoString(i) + "_" + intoString(j);
      lights.yellow[j] = createPanel(id, a_hud_part_design.name + "_yellow",
                                     offset + step * (0.5 + j) - width / 2 + 1,
                                     size.second * i, width, size.second, lights_container);
      // red
      id = a_hud_part_design.name + "_red_" + intoString(i) + "_" + intoString(j);
      lights.red[j] = createPanel(id, a_hud_part_design.name + "_red",
                                  offset + step * (0.5 + j) - width / 2 - 1, size.second * i,
                                  width, size.second, lights_container);
    }

    tab_lights_elements.push_back(lights);

    // text for diasplaying weapon names
    // get the weapon shortened name
    string weapon_name = Game::text->getText(weapons[i]->weapon_design.text_list_name);
    id = a_hud_part_design.name + "_name_text_" + intoString(i); // unique id
    tab_name_elements.push_back(createTextArea(id, weapon_name, font_size,
                                Game::hud->hud_design.display_colours[0],
                                offset + 1, size.second * i + 3,
                                size.first - offset, size.second,
                                text_container));

    // text for diasplaying ammo number
    id = a_hud_part_design.name + "_ammo_text_" + intoString(i); // unique id
    tab_ammo_elements.push_back(createTextArea(id, "000", font_size,
                                Game::hud->hud_design.display_colours[0],
                                11, size.second * i + 3, offset,
                                size.second, text_container));
  }
}

/** @brief displays weapons, their ammo, their charge status
  */
void HudWeaponTab::update(Ogre::Real a_dt)
{
  accumulator += a_dt;
  if (accumulator > interval) { // runs at 10fps
    // get the addres of the vector of weapon indexes in the selected group
    vector<usint> current_group = Game::hud->player_unit->getSelectedGroup();
    usint g = 0; // index of weapons in  the group

    // deafult to no weapon selected - in group mode all weapons are selected in group
    usint current_weapon = -1;

    // if in single mode find the current weapon
    if (!Game::hud->controller->control_block.fire_mode_group) {
      current_weapon = Game::hud->player_unit->getSelectedWeapon();
    }

    // go trhough all weapons to show the ammo and apprpriate lights under the name
    for (usint i = 0, for_size = weapons.size(); i < for_size; ++i) {
      // get the number of shots left in weapon
      ulint ammo = weapons[i]->getAmmo();

      // set the ammo counter text
      if (ammo > 0) {
        stringstream stream;
        string ammo_string;
        stream << setw(3) << setfill('0') << ammo; // prepare the string "001"
        stream >> ammo_string;
        tab_ammo_elements[i]->setCaption(ammo_string);

      } else {
        // out of ammo
        tab_ammo_elements[i]->setCaption("---");
        tab_ammo_elements[i]->setColour(Game::hud->hud_design.display_colours[1]);
      }

      // determine if the weapon is in the selected group
      bool in_selected_group = false;
      if (g < current_group.size() && current_group[g] == i) { // if weapon in the group
        in_selected_group = true; // set active
        g++; // advance one - the vector is sorted so this works
      }

      // light up functional weapons
      if (weapons[i]->isOperational()) { // is the weapon still capable of firing
        // determine how many lights to light to show the charging progress
        Ogre::Real charged = weapons[i]->getCharge() * number_of_lights;

        // light up the tri-colour lights under the weapon name
        if (in_selected_group) {
          // show selected group in green - except the selected weapon which is in yellow
          if (Game::hud->controller->control_block.fire_mode_group
              || i != current_weapon) {
            // groups in green
            for (usint j = 0; j < number_of_lights; ++j) {
              if (j < charged) {
                tab_lights_elements[i].green[j]->show();

              } else {
                tab_lights_elements[i].green[j]->hide();
              }
              tab_lights_elements[i].yellow[j]->hide();
              tab_lights_elements[i].red[j]->hide();
            }
          } else {
            // single selected weapon in yellow
            for (usint j = 0; j < number_of_lights; ++j) {
              tab_lights_elements[i].green[j]->hide();
              if (j < charged) {
                tab_lights_elements[i].yellow[j]->show();

              } else {
                tab_lights_elements[i].yellow[j]->hide();
              }
              tab_lights_elements[i].red[j]->hide();
            }
          }
        } else {
          // if the weaopn is not in the current group show in red
          for (usint j = 0; j < number_of_lights; ++j) {
            tab_lights_elements[i].green[j]->hide();
            tab_lights_elements[i].yellow[j]->hide();
            if (j < charged) {
              tab_lights_elements[i].red[j]->show();

            } else {
              tab_lights_elements[i].red[j]->hide();
            }
          }
        }
      } else {
        // black out the lights for non-operational weapons
        for (usint j = 0; j < number_of_lights; ++j) {
          tab_lights_elements[i].green[j]->hide();
          tab_lights_elements[i].yellow[j]->hide();
          tab_lights_elements[i].red[j]->hide();
        }
      }
    }

    // reset the accumulator for the next frame
    accumulator = 0;
  }
}
