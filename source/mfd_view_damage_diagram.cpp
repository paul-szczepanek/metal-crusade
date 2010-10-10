//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "mfd_view_damage_diagram.h"
#include "game.h"
#include "hud.h"
#include "unit.h"

//this is pretty much the bottom level of the MFD,
//stuff here is specific to one hud - the military hud, it's pretty much hardcoded
//I did my best to make it a modular slot in so it's not tied to anything and can be easily
//replaced with a different view - I'm just running out of time and need to get the hud working

//TODO:move this to a config file
const int dmg_pic_w = 128;
const int dmg_ico_w = 64;

const vector<string> military_blank_grn_tex_names = {
    "mfd_dmg_grn_blank"
};

const vector<string> military_blank_red_tex_names = {
    "mfd_dmg_red_blank"
};

const vector<string> military_object_grn_tex_names = {
    "mfd_dmg_grn_object"
};

const vector<string> military_object_red_tex_names = {
    "mfd_dmg_red_object"
};

const vector<string> military_biped_crusader_grn_tex_names = {
    "mfd_dmg_grn_tc",
    "mfd_dmg_grn_tr",
    "mfd_dmg_grn_tl",
    "mfd_dmg_grn_ar",
    "mfd_dmg_grn_al",
    "mfd_dmg_grn_lr",
    "mfd_dmg_grn_ll",
    "mfd_dmg_grn_tcb",
    "mfd_dmg_grn_trb",
    "mfd_dmg_grn_tlb"
};

const vector<string> military_biped_crusader_red_tex_names = {
    "mfd_dmg_red_tc",
    "mfd_dmg_red_tr",
    "mfd_dmg_red_tl",
    "mfd_dmg_red_ar",
    "mfd_dmg_red_al",
    "mfd_dmg_red_lr",
    "mfd_dmg_red_ll",
    "mfd_dmg_red_tcb",
    "mfd_dmg_red_trb",
    "mfd_dmg_red_tlb"
};

const usint num_of_damage_levels = 5;

const vector<Ogre::Real> dmg_level_flashing = {
    0.1,
    0.5,
    0,
    0,
    0,
    0, //destroyed
};

const vector<real_pair> dmg_level_colour_values = {
    make_pair(0, 1),        //<0.2
    make_pair(0.25, 1),     //<0.4
    make_pair(0.25, 1),     //<0.6
    make_pair(1, 0.75),     //<0.8
    make_pair(1, 0),        //<1.0
    make_pair(0, 0.25),     //=destroyed
};

MFDViewDamageDiagram::MFDViewDamageDiagram(hud_part_design_t& a_hud_part_design)
    : MFDView(a_hud_part_design), diagram(mfd_view::biped_crusader), flashing_accumulator(0)
{
    Ogre::OverlayManager* overlay_mngr = Ogre::OverlayManager::getSingletonPtr();

    //for positioning pics
    offset_x = (size.first - dmg_pic_w) * 0.5;
    ico_offset_y = size.second - dmg_ico_w;

    //creates a container icons
    icons = static_cast<Ogre::OverlayContainer*>
                    (overlay_mngr->createOverlayElement("Panel", a_hud_part_design.name
                                                        +"_icons_cont_"+getUniqueID()));
    icons->setMetricsMode(Ogre::GMM_PIXELS);
    container->addChild(icons);

    //creates a container the heat gauge
    heat_gauge = static_cast<Ogre::OverlayContainer*>
                    (overlay_mngr->createOverlayElement("Panel", a_hud_part_design.name
                                                        +"_heat_cont_"+getUniqueID()));
    heat_gauge->setMetricsMode(Ogre::GMM_PIXELS);
    container->addChild(heat_gauge);

    //icons overlay elements
    string id = "mfd_dmg_grn_electronics";
    ico_electronics.first = createPanel(id+getUniqueID(), id,
                                        offset_x + dmg_ico_w * 0.5, ico_offset_y,
                                        dmg_ico_w, dmg_ico_w, icons);

    id = "mfd_dmg_red_electronics";
    ico_electronics.second = createPanel(id+getUniqueID(), id,
                                         offset_x + dmg_ico_w * 0.5, ico_offset_y,
                                         dmg_ico_w, dmg_ico_w, icons);

    id = "mfd_dmg_grn_engine";
    ico_engine.first = createPanel(id+getUniqueID(), id, offset_x + dmg_ico_w * 0.5, ico_offset_y,
                                   dmg_ico_w, dmg_ico_w, icons);

    id = "mfd_dmg_red_engine";
    ico_engine.second = createPanel(id+getUniqueID(), id, offset_x + dmg_ico_w * 0.5, ico_offset_y,
                                    dmg_ico_w, dmg_ico_w, icons);

    id = "mfd_dmg_grn_lock";
    ico_lock_in.first = createPanel(id+getUniqueID(), id, offset_x + dmg_ico_w * 1, ico_offset_y,
                                    dmg_ico_w, dmg_ico_w, icons);

    id = "mfd_dmg_red_lock";
    ico_lock_in.second = createPanel(id+getUniqueID(), id, offset_x + dmg_ico_w * 1, ico_offset_y,
                                     dmg_ico_w, dmg_ico_w, icons);

    //heat bar overlay elements
    id = "mfd_dmg_heat";
    heat = createPanel(id+getUniqueID(), id, offset_x, ico_offset_y,
                       dmg_ico_w * 0.5, dmg_ico_w, heat_gauge);

    id = "mfd_dmg_heat_bar"+getUniqueID();
    string tex_name = "mfd_dmg_heat_bar";

    //get the material to scale the bar later
    heat_bar_material = Game::hud->createOverlayMaterial(id, Ogre::TextureUnitState::TAM_CLAMP,
                                                         tex_name)->getTechnique(0)->getPass(0)
                                                         ->getTextureUnitState(0);

    heat_bar = createPanel(id, id, offset_x, ico_offset_y,
                           dmg_ico_w * 0.5, dmg_ico_w, heat_gauge);

    //creates containers for diagram types
    for (usint i = 0; i < mfd_view::num_of_diagram_types; ++i) {
        diagrams.push_back(static_cast<Ogre::OverlayContainer*>
                    (overlay_mngr->createOverlayElement("Panel", a_hud_part_design.name+"_cont_"
                                                        +Game::intIntoString(i)+getUniqueID())));
        diagrams.back()->setMetricsMode(Ogre::GMM_PIXELS);
        container->addChild(diagrams.back());
    }

    //create the damage diagrams
    createDiagram(mfd_view::blank, military_blank_grn_tex_names, military_blank_red_tex_names,
                  material_blank_grns, material_blank_reds);

    createDiagram(mfd_view::object, military_object_grn_tex_names, military_object_red_tex_names,
                  material_object_grns, material_object_reds);

    createDiagram(mfd_view::biped_crusader, military_biped_crusader_grn_tex_names,
                  military_biped_crusader_red_tex_names, material_biped_crusader_grns,
                  material_biped_crusader_reds);

    //set initial diagram on
    switchDiagrams(diagram);
    //deactivate
    activate(false);
}

/** @brief creates a vectot of texture states of a damage pic to use to change alpha blending
  */
void MFDViewDamageDiagram::createDiagram(mfd_view::diagram_type a_diagram_type,
                                         const vector<string>& grn_names,
                                         const vector<string>& red_names,
                                         vector<Ogre::TextureUnitState*>& materials_grn,
                                         vector<Ogre::TextureUnitState*>& materials_red)
{
    Ogre::MaterialPtr material;

    //create elements and extracts texture units to use later for changing alpha blending
    for (usint i = 0, for_size = grn_names.size(); i < for_size; ++i) {
        //get uniqe id and the texture from the design vector
        string tex_name = grn_names[i];
        string id = grn_names[i]+getUniqueID();

        //create a material witha a unique names but the texture name from the mfd_view diagram
        material = Game::hud->createOverlayMaterial(id, Ogre::TextureUnitState::TAM_CLAMP,
                                                    tex_name);

        //put the texture unit state in the vector for later use with alpha blending
        materials_grn.push_back(material->getTechnique(0)->getPass(0)->getTextureUnitState(0));

        //creat the overlay elemnt to apply it to
        createPanel(id, id, offset_x, offset_x, dmg_pic_w, dmg_pic_w,
                    diagrams[a_diagram_type]);

        //get uniqe id and the texture from the design vector
        tex_name = red_names[i];
        id = red_names[i]+getUniqueID();

        //create a material witha a unique names but the texture name from the mfd_view diagram
        material = Game::hud->createOverlayMaterial(id, Ogre::TextureUnitState::TAM_CLAMP,
                                                    tex_name);

        //put the texture unit state in the vector for later use with alpha blending
        materials_red.push_back(material->getTechnique(0)->getPass(0)->getTextureUnitState(0));

        //creat the overlay elemnt to apply it to
        createPanel(id, id, offset_x, offset_x, dmg_pic_w, dmg_pic_w,
                    diagrams[a_diagram_type]);
    }

    //add the vectors of texture units to the vector of vectors used to pick the active diagram
    materials_grns.push_back(&materials_grn);
    materials_reds.push_back(&materials_red);
}

/** @brief change the damage diagram picture to suite the object type
  */
void MFDViewDamageDiagram::switchDiagrams(mfd_view::diagram_type a_new_diagram)
{
    diagram = a_new_diagram;

    for (usint i = 0; i < mfd_view::num_of_diagram_types; ++i) {
        if (i == diagram) {
            //show the container with the selected view
            diagrams[i]->show();

        } else {
            //hide all else
            diagrams[i]->hide();
        }
    }

    //if blank hide the icons as well
    if (diagram == mfd_view::blank) {
        heat_gauge->hide();
        icons->hide();

    } else {
        heat_gauge->show();
        icons->show();
    }
}

void MFDViewDamageDiagram::update(Ogre::Real a_dt)
{
    if (active) {
        updateDiagramElements(a_dt, Game::hud->player_unit);
    }
}

void MFDViewDamageDiagram::updateDiagramElements(Ogre::Real a_dt, Mobilis* a_target)
{
    mfd_view::diagram_type new_diagram = a_target->getDiagramType();

    //switch diagrams if needed
    if (new_diagram != diagram) switchDiagrams(new_diagram);

    //heat guage
    Ogre::Real temperature = a_target->getSurfaceTemperature() / 1000;
    if (temperature > 1) temperature = 1; //cap at 1

    //scale the height of the heat gauge
    Ogre::Real heat_height = dmg_ico_w * temperature;
    heat_bar->setHeight(heat_height);
    heat_bar->setPosition(offset_x, ico_offset_y + dmg_ico_w - heat_height);

    //change UV mapping accordingly
    heat_bar_material->setTextureVScale(1 / temperature);
    heat_bar_material->setTextureVScroll(0.5 * (1 - temperature));

    for (usint i = 0, for_size = mfd_view::num_of_diagram_elements[diagram]; i < for_size; ++i) {
        //get the damage for each area corresponding to the diagam elemnt
        Ogre::Real damage = a_target->getDamage(i);

        //special case - part has been destroyed if damage < 0
        usint damage_index = num_of_damage_levels;

        if (damage >= 0) {
            //get the index of the damage colours vector rounded down - hence the * 0.9
            damage_index = damage * num_of_damage_levels * 0.99999;

            //if damage at a certain level start flashing
            if (dmg_level_flashing[damage_index] > 0) {
                //use flashing_accumulator to keep track of flashing
                flashing_accumulator += a_dt;

                //after the same interval reset the flashing process
                if (flashing_accumulator > 2 * dmg_level_flashing[damage_index]) {
                    flashing_accumulator = 0;
                } else if (flashing_accumulator > dmg_level_flashing[damage_index]) {
                    //if larger then the set interval hide the flashing red element
                    damage_index = num_of_damage_levels;
                }
            }
        }

        //apply alpha values to the green elements
        (*materials_grns[diagram])[i]->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
                                Ogre::LBS_TEXTURE, dmg_level_colour_values[damage_index].first);

        //apply alpha values to the red elements
        (*materials_reds[diagram])[i]->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL,
                            Ogre::LBS_TEXTURE, dmg_level_colour_values[damage_index].second);
    }
}
