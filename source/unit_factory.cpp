//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "unit_factory.h"
#include "files_handler.h"
#include "query_mask.h"
#include "game.h"
#include "crusader.h"
#include "unit.h"

UnitFactory::UnitFactory()
{
    //ctor
}

/** @brief destroys all units and ogre objects
  * @todo: destroy ogre objects
  */
UnitFactory::~UnitFactory()
{
    for (list<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
        //delete entities (meshes)
        destroyModel((*it)->scene_node);
        delete *it;
    }

    units.clear();
}

void UnitFactory::updateUnits(Ogre::Real a_dt)
{
    //call update on every unit
    for (list<Unit*>::iterator it = units.begin(); it != units.end(); ) {
        //remove if expired
        if ((*it)->update(a_dt) == 1) {
            //destroy the Ogre part
            destroyModel((*it)->scene_node);

            //destroy the unit
            delete *it;

            //get the iterator to the next item after removal
            it = units.erase(it);

        } else {
            ++it;
        }
    }
}

Unit* UnitFactory::getUnit(unsigned int a_id)
{
    //TEMP!!!!!!!!
    list<Unit*>::iterator it_end = units.end();
    for (list<Unit*>::iterator it = units.begin(); it != it_end; ) {
        return *it;
    }
    return *it_end;
}

/** @brief creates units and add's them to a list
  */
Crusader* UnitFactory::spawnCrusader(Ogre::Vector3 a_pos_xyz, const string& a_name,
                                     Ogre::Quaternion a_orientation)
{
    //structs for spec for crusader
    crusader_engine_t engine;
    crusader_drive_t drive;
    crusader_chasis_t chasis;
    crusader_model_t model;
    crusader_design_t design;

    //load spec from file
    FilesHandler::getCrusaderDesign(a_name, design, engine, drive, chasis, model);

    //get unique string from id
    string id_string = getUniqueID()+'_'+chasis.mesh; //append unique id to name

    //create entities
    Ogre::Entity* drive_mesh = Game::scene->createEntity(id_string+"_drive",
                                                                 drive.mesh+"_drive.mesh");
    Ogre::Entity* chasis_mesh = Game::scene->createEntity(id_string+"_chasis",
                                                                  chasis.mesh+"_chasis.mesh");
    Ogre::Entity* arm_right_mesh = Game::scene->createEntity(id_string+"_arm_right",
                                                                     chasis.mesh+"_arm_right.mesh");
    Ogre::Entity* arm_left_mesh = Game::scene->createEntity(id_string+"_arm_left",
                                                                    chasis.mesh+"_arm_left.mesh");
    Ogre::Entity* leg_right_mesh = Game::scene->createEntity(id_string+"_leg_right",
                                                                     drive.mesh+"_leg_right.mesh");
    Ogre::Entity* leg_left_mesh = Game::scene->createEntity(id_string+"_leg_left",
                                                                    drive.mesh+"_leg_left.mesh");
    //assign materials
    drive_mesh->setMaterialName(design.material);
    chasis_mesh->setMaterialName(design.material);
    arm_left_mesh->setMaterialName(design.material);
    arm_right_mesh->setMaterialName(design.material);
    leg_left_mesh->setMaterialName(design.material);
    leg_right_mesh->setMaterialName(design.material);

    //create root node (drive) and attach chasis node as a child
    Ogre::SceneNode* crusader_node = Game::scene->getRootSceneNode()->createChildSceneNode();
    Ogre::SceneNode* chasis_node = crusader_node->createChildSceneNode();
    Ogre::SceneNode* arm_right_node = chasis_node->createChildSceneNode(); //atach arms to chasis
    Ogre::SceneNode* arm_left_node = chasis_node->createChildSceneNode();

    //attach meshes
    crusader_node->attachObject(drive_mesh);
    chasis_node->attachObject(chasis_mesh);
    arm_right_node->attachObject(arm_right_mesh);
    arm_left_node->attachObject(arm_left_mesh);
    crusader_node->attachObject(leg_right_mesh); //legs attached directly to drive
    crusader_node->attachObject(leg_left_mesh);

    //set flags to entities
    drive_mesh->setQueryFlags(query_mask_units);
    chasis_mesh->setQueryFlags(query_mask_units);
    arm_left_mesh->setQueryFlags(query_mask_units);
    arm_right_mesh->setQueryFlags(query_mask_units);
    leg_left_mesh->setQueryFlags(query_mask_units);
    leg_right_mesh->setQueryFlags(query_mask_units);

    //pass the root node (drive) to the crusader object
    Crusader* spawn = new Crusader(a_pos_xyz, chasis.mesh, crusader_node, a_orientation,
                                   design, engine, drive, chasis, model);

    //put the unit on the list
    units.push_back(spawn);

    return spawn;
}


