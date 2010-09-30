//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "hud.h"
#include "game.h"
#include "unit.h"
#include "camera.h"
#include "input_handler.h"
#include "files_handler.h"
#include "game_controller.h"
#include "hud_dial.h"
#include "hud_status_display.h"
#include "hud_log_printer.h"
#include "hud_pointer.h"
#include "hud_set.h"
#include "hud_text.h"
#include "hud_weapon_tab.h"
#include "hud_mfd_aux_display.h"
#include "hud_mfd_display.h"
#include "mfd_computer.h"
#include "log_computer.h"
#include "status_computer.h"

const string hud_textures_dir = "data/texture/hud/";

//hud designs in pixel coords baselined at 1024x768
const Ogre::Real base_width(1024);
const Ogre::Real base_height(768);

Hud::Hud()
    : active(false), hud_width(base_width), hud_height(base_height),
    scale(1), scale_w(1), scale_h(1), selected_mfd(0)
{
    log = new LogComputer();
    status = new StatusComputer();
}

/** @brief unloads the whole frontend when exiting the arena
  */
Hud::~Hud()
{
    //destroy all the parts
    for (usint i = 0, for_size = hud_parts.size(); i < for_size; ++i) {
        delete hud_parts[i];
    }

    //destroy all overlays an elements, this also takes care of ogre elements created by hud_parts
    Ogre::OverlayManager* overlay_mngr = &Ogre::OverlayManager::getSingleton();
    overlay_mngr->destroyAllOverlayElements();
    overlay_mngr->destroyAll();

    //release materials for the hud as no longer needed
    Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("hud");

    delete status;
    delete log;
}

/** @brief turns hud on/off
  */
void Hud::activate(bool a_toggle)
{
    active = a_toggle;
    if (active) {
        //show the overlays
        for (usint i = 0; i < hud_num_of_areas; ++i) {
            hud_overlays[i]->show();
            hud_overlay_3d->show();
        }

        //init log messages TODO: get info from arena
        //get time //get arena name //get coords
        log->changeLine("$a1602 $rentering sandy planes at $rN$e30$xE$e08");
        log->addLine("$r1602 $atemperature $e60'C $esand storm $rincoming"); //get info from arena

    } else {
        //hide the overlays
        for (usint i = 0; i < hud_num_of_areas; ++i) {
            hud_overlays[i]->hide();
            hud_overlay_3d->hide();
        }
    }
}

/** @brief actually loads the hud with all the textures dependent on the name
  * all assets are later fried up when the hud is killed as they are big textures
  */
void Hud::loadHud(Unit* a_player_unit)
{
    //hook up the hud to the unit
    player_unit = a_player_unit;
    player_unit->attachHud(true);

    //alias the controller for hud parts access
    controller = player_unit->getController();

    //hook up the timer
    timer = Game::instance()->getTimer();

    //try and load the definition
    if (FilesHandler::getHudDesign(player_unit->getHudName(), hud_design) == false) {
        Game::kill("hud definition failed to load!");
    }

    //load hud materials from a directory based on the name of the hud from the mech design
    Ogre::ResourceGroupManager& resource_mngr = Ogre::ResourceGroupManager::getSingleton();
    resource_mngr.addResourceLocation(hud_textures_dir+player_unit->getHudName(),
                                      "FileSystem", "hud");
    resource_mngr.initialiseResourceGroup("hud");

    //get the overlay manager
    Ogre::OverlayManager* overlay_mngr = &Ogre::OverlayManager::getSingleton();

    //create overlay and containers for all areas and add them to the overlay
    for (usint i = 0; i < hud_num_of_areas; ++i) {
        //read in initial offsets from definition
        area_offsets[i] = hud_design.offsets[i];

        //create one overlay per area
        hud_overlays[i] = overlay_mngr->create(hud_area_names[i]);

        //create materials for the hud areas
        createOverlayMaterial(hud_area_names[i]);

        //create the top container
        hud_areas[i] = (static_cast<Ogre::OverlayContainer*>
                        (overlay_mngr->createOverlayElement("Panel",
                                                            hud_area_names[i]+"_hud_area")));

        //apply meterial with texture from design
        hud_areas[i]->setMaterialName(hud_design.area_textures[i]);

        //set size and position
        hud_areas[i]->setMetricsMode(Ogre::GMM_PIXELS);
        hud_areas[i]->setDimensions(hud_design.sizes[i].first,
                                    hud_design.sizes[i].second);
        hud_areas[i]->setPosition(0,0);

        //add to the overlay
        hud_overlays[i]->add2D(hud_areas[i]);
        hud_overlays[i]->setZOrder(400);
    }

    //overlay for 3d objects
    hud_overlay_3d = overlay_mngr->create("3doverlay");
    hud_overlay_3d->setZOrder(300);

    //create individual hud parts which hook up to areas
    for (usint i = 0, for_size = hud_design.parts.size(); i < for_size; ++i) {
        if (hud_design.parts[i].type == hud_part_enum::mfd_military) {
            //create the mfd first
            mfds.push_back(new MFDComputer());

            //then create the hud part that will us it
            hud_parts.push_back(new HudMFDDisplay(hud_design.parts[i]));

        } else if (hud_design.parts[i].type == hud_part_enum::mfd_aux_military) {
            if (mfds.size() > 0) {
                //this will used the most recently created MFD computer
                hud_parts.push_back(new HudMFDAuxDisplay(hud_design.parts[i]));

            } else {
                //oh, dear, the hud design is borked
                Game::kill(string("this mfdaux needs an mfd def first: ")+hud_design.parts[i].name);
            }
        } else if (hud_design.parts[i].type == hud_part_enum::radar_military) {
            //

        } else if (hud_design.parts[i].type == hud_part_enum::set) {
            hud_parts.push_back(new HudSet(hud_design.parts[i]));

        } else if (hud_design.parts[i].type == hud_part_enum::pointer) {
            hud_parts.push_back(new HudPointer(hud_design.parts[i]));

        } else if (hud_design.parts[i].type == hud_part_enum::weapon_tab) {
            hud_parts.push_back(new HudWeaponTab(hud_design.parts[i]));

        } else if (hud_design.parts[i].type == hud_part_enum::text) {
            hud_parts.push_back(new HudText(hud_design.parts[i]));

        } else if (hud_design.parts[i].type == hud_part_enum::log_printer) {
            hud_parts.push_back(new HudLogPrinter(hud_design.parts[i]));

        } else if (hud_design.parts[i].type == hud_part_enum::dial) {
            hud_parts.push_back(new HudDial(hud_design.parts[i]));

        } else if (hud_design.parts[i].type == hud_part_enum::status_display) {
            hud_parts.push_back(new HudStatusDisplay(hud_design.parts[i]));
        }
    }

    //set size and position of all the elements
    resize(Game::render_window->getWidth(), Game::render_window->getHeight());
}

/** @brief slides overlays around smoothly to match their current offset
  */
void Hud::offsetUpdate(Ogre::Real a_dt, hud_area a_hud_area, bool a_alternative)
{
    //smoth transition bewteen offsets
    if (a_alternative) {
        area_offsets[a_hud_area].first = (area_offsets[a_hud_area].first
                                          + hud_design.offsets_alternative[a_hud_area].first) * 0.5;
        area_offsets[a_hud_area].second = (area_offsets[a_hud_area].second
                                           + hud_design.offsets_alternative[a_hud_area].second)
                                           * 0.5;
    } else {
        area_offsets[a_hud_area].first = (area_offsets[a_hud_area].first
                                          + hud_design.offsets[a_hud_area].first) * 0.5;
        area_offsets[a_hud_area].second = (area_offsets[a_hud_area].second
                                           + hud_design.offsets[a_hud_area].second) * 0.5;
    }

    //reposition
    Ogre::Real x = positionHorizontal(hud_design.sizes[a_hud_area].first,
                                      hud_design.positions[a_hud_area].first,
                                      area_offsets[a_hud_area].first);
    Ogre::Real y = positionVertical(hud_design.sizes[a_hud_area].second,
                                    hud_design.positions[a_hud_area].second,
                                    area_offsets[a_hud_area].second);

    hud_overlays[a_hud_area]->setScroll(x, -y);
}

/** @brief main loop - updates all parts and deals with mode selection and handles mfd selection
  * the fps of each part is independent, each has an accumulator; digital parts can run at lower fps
  */
void Hud::update(Ogre::Real a_dt)
{
    if (active) {
        //mfd selection
        if (Game::take(Game::hud->controller->control_block.mfd1_select)) {
            //select previous mfd
            mfds[selected_mfd]->activate(false);
            --selected_mfd;
            if (selected_mfd > mfds.size()) {
                selected_mfd = 0;
            }
            mfds[selected_mfd]->activate(true);

        } else if (Game::take(Game::hud->controller->control_block.mfd2_select)) {
            //select next mfd
            mfds[selected_mfd]->activate(false);
            ++selected_mfd;
            if (selected_mfd > mfds.size() - 1) {
                selected_mfd = mfds.size() - 1;
            }
            mfds[selected_mfd]->activate(true);
        }

        //slide the log and status computer if active TODO: do as lazy update
        if (player_unit->getHudMode() == interface_mode::log) {
            offsetUpdate(a_dt, hud_log, false);
            mfds[selected_mfd]->activate(false);
        } else {
            offsetUpdate(a_dt, hud_log, true);
        }

        if (player_unit->getHudMode() == interface_mode::computer) {
            offsetUpdate(a_dt, hud_status, false);
            mfds[selected_mfd]->activate(false);
        } else {
            offsetUpdate(a_dt, hud_status, true);
        }

        //update all the parts
        for (usint i = 0, for_size = hud_parts.size(); i < for_size; ++i) {
            hud_parts[i]->update(a_dt);
        }
    }
}

/** @brief resizes and positions all the elements again to accomodate any sreens size
  */
void Hud::resize(unsigned int a_screen_width, unsigned int a_screen_height)
{
    //get scale from the screen width and height
    hud_width = a_screen_width;
    hud_height = a_screen_height;

    //hud baselined for 1024x768 (I know, I'm rocking like it's 1996)
    scale_w = hud_width / base_width;
    scale_h = hud_height / base_height;

    //use the smaller scale so there's no overlap neither vertically nor horizontaly
    scale = min(scale_w, scale_h);

    //reposition and rescale containers
    for (usint i = 0; i < hud_num_of_areas; ++i) {
        hud_overlays[i]->setScale(scale, scale);
        Ogre::Real x = positionHorizontal(hud_design.sizes[i].first,
                                          hud_design.positions[i].first,
                                          area_offsets[i].first);
        Ogre::Real y = positionVertical(hud_design.sizes[i].second,
                                        hud_design.positions[i].second,
                                        area_offsets[i].second);

        hud_overlays[i]->setScroll(x, -y);
    }

    //rescale and reposition all the 3d elements in hud parts
    for (usint i = 0, for_size = hud_parts.size(); i < for_size; ++i) {
        hud_parts[i]->resize(scale);
    }
}

/** @brief creates a metarial and apply a texture to it of the same name
  * if there is already a material of same name it returns that instead
  */
Ogre::MaterialPtr Hud::createOverlayMaterial(const string& a_name, texture_addressing a_addressing,
                                             string a_texture_name)
{
    Ogre::MaterialPtr material;

    if (a_texture_name.empty()) {
        a_texture_name = a_name;
    }

    //check if it already exists
    if (Ogre::MaterialManager::getSingleton().resourceExists(a_name)) {
        //if it does just get the pointer by name and return that
        material = Ogre::MaterialManager::getSingleton().getByName(a_name);

    } else {
        //if it doesn't exist yet create the material with the given name
        material = Ogre::MaterialManager::getSingleton().create(a_name, "hud");

        //the material is created manually, has only one pass with a diffuse texture
        Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
        pass->createTextureUnitState()->setTextureName(a_texture_name+".dds");

        //enable alpha blending
        pass->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
        //crucial bit for the hud - enable clamping by default
        pass->getTextureUnitState(0)->setTextureAddressingMode(a_addressing);
        //get them a bit cristper
        pass->setTextureFiltering(Ogre::TFO_TRILINEAR);
    }

    return material;
}

/** @brief parses a string and seperates it into three strings - one for each colour
  * this makes the number of hud colours hardcoded at three - but this is the only cap
  * this cap can be removed by making the function accept any number of lines
  * but at a cost of complexity and more checking so I'll do that if I ever need more than 3 colours
  */
void Hud::parseColours(const string& message, usint a_length,
                       char* line1, char* line2, char* line3)
{
    usint colour = 0; //default colour
    usint last_colour = colour; //previously used colour
    usint k = 0; //string position
    usint k_limit = message.length();

    //parse the string and seperate the message into different colours
    for (usint i = 0; i < a_length; ++i) {
        //if you hit an escape char
        if (k < k_limit && message[k] == hud_escape_char) {
            //move to next char
            ++k;

            if (k < k_limit) { //check in case the escape char was the last thing in the string
                //if revert colour char found
                if (message[k] == hud_revert_colour_char) {
                    //switch to the previous colour
                    colour = last_colour;

                    //skip the revert colour char
                    ++k;

                } else {
                    //otherwise look for a new colour code
                    for (usint l = 0; l < hud_num_of_colours; ++l) {
                        //set the colour if the colour cound is found
                        if (message[k] == hud_colour_codes[l]) {
                            //push old colour as the last used colour
                            last_colour = colour;
                            colour = l; //and set the new colour

                            //skip the colour char
                            ++k;

                            break;
                        }
                    }
                }
            }
        }

        //insert chars into appropriate lines
        if (k < k_limit) {
            //if there's still enough string put the chars in the appropriate line - blank the rest
            line1[i] = (colour == 0)? message[k] : ' ';
            line2[i] = (colour == 1)? message[k] : ' ';
            line3[i] = (colour == 2)? message[k] : ' ';

            //progress the string position
            ++k;

        } else {
            //otherwise blank all the chars
            line1[i] = ' ';
            line2[i] = ' ';
            line3[i] = ' ';
        }
    }
}

/** @brief translate positions given in enums into OGRE relative coords
  * overlays in OGRE are fucked up and the documentation is utterly misleading
  * the moving is x,-y not x,y and bizzarely the screen size is <0,2> and not <0,1>
  * scaling is done after movement against the point 1,-1 not 0.5,0.5
  */
Ogre::Real Hud::positionHorizontal(int a_width, horizontal::position a_position,
                                   Ogre::Real a_offset)
{
    switch (a_position) {

    case horizontal::left:
        return (0 - (1 - scale)) - (a_offset * scale) / hud_width;

    case horizontal::centre:
        return 1 - (1 - scale) - ((a_width + 2 * a_offset) * scale) / hud_width;

    case horizontal::right:
        return 2 - (1 - scale) - (2 * (a_width + a_offset) * scale) / hud_width;// + (1 - scale);

    default:
        return 0;
    }
}

/** @brief translate positions given in enums into OGRE relative coords
  * see positionHorizontal above
  */
Ogre::Real Hud::positionVertical(int a_height, vertical::position a_position, Ogre::Real a_offset)
{
    switch (a_position) {

    case vertical::top:
        //negative offset to appease the OGRE
        return 0 - (1 - scale) + (2 * a_offset * scale / hud_height);

    case vertical::centre:
        return 1 - (1 - scale) - ((a_height - a_offset) * scale) / hud_height;

    case vertical::bottom:
        return 2 - (1 - scale) - (2 * (a_height - a_offset) * scale) / hud_height ;

    default:
        return 0;
    }
}

/** @brief translate positions given in enums into pixel coords
  */
Ogre::Real Hud::getHudAreaOriginX(hud_area a_hud_area)
{

    switch (hud_design.positions[a_hud_area].first) {

    case horizontal::left:
        return area_offsets[a_hud_area].first * scale;

    case horizontal::centre:
        return hud_width - (hud_design.sizes[a_hud_area].first + area_offsets[a_hud_area].first)
               * 0.5 * scale;

    case horizontal::right:
        return hud_width - (hud_design.sizes[a_hud_area].first + area_offsets[a_hud_area].first)
               * scale;

    default:
        return 0;
    }
}

/** @brief translate positions given in enums into pixel coords
  */
Ogre::Real Hud::getHudAreaOriginY(hud_area a_hud_area)
{
    switch (hud_design.positions[a_hud_area].second) {

    case vertical::top:
        return area_offsets[a_hud_area].second * scale;

    case vertical::centre:
        return hud_height - (hud_design.sizes[a_hud_area].second + area_offsets[a_hud_area].second)
               * 0.5 * scale;

    case vertical::bottom:
        return hud_height - (hud_design.sizes[a_hud_area].second + area_offsets[a_hud_area].second)
               * scale;

    default:
        return 0;
    }
}
