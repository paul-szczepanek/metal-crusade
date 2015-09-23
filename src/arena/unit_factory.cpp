// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "unit_factory.h"
#include "files_handler.h"
#include "query_mask.h"
#include "game.h"
#include "crusader.h"
#include "unit.h"

UnitFactory::UnitFactory()
{
}

/** @brief destroys all units and ogre objects
 */
UnitFactory::~UnitFactory()
{
  for (list<Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
    delete *it;
  }
  units.clear();
}

Unit* UnitFactory::getUnit(uint a_id)
{
  // TEMP!!!!!!!!
  list<Unit*>::iterator it_end = units.end();
  for (list<Unit*>::iterator it = units.begin(); it != it_end; ) {
    return *it;
  }
  return *it_end;
}

/** @brief creates units and add's them to a list
 */
Crusader* UnitFactory::spawnCrusader(Vector3       a_pos_xyz,
                                     const string& a_name,
                                     Quaternion    a_orientation)
{
  // structs for spec for crusader
  crusader_engine_t engine;
  crusader_drive_t drive;
  crusader_chasis_t chasis;
  crusader_design_t design;

  // load spec from file
  FilesHandler::getCrusaderDesign(a_name, design, engine, drive, chasis);

  // get unique string from id
  string id_string = Game::getUniqueID() + '_' + chasis.mesh; // append unique id to name

  // create entities
  Ogre::Entity* drive_mesh = Game::scene->createEntity(id_string + "_drive",
                                                       drive.mesh + "_drive.mesh");
  Ogre::Entity* chasis_mesh = Game::scene->createEntity(id_string + "_chasis",
                                                        chasis.mesh + "_chasis.mesh");
  Ogre::Entity* arm_right_mesh = Game::scene->createEntity(id_string + "_arm_right",
                                                           chasis.mesh + "_arm_right.mesh");
  Ogre::Entity* arm_left_mesh = Game::scene->createEntity(id_string + "_arm_left",
                                                          chasis.mesh + "_arm_left.mesh");
  Ogre::Entity* leg_right_mesh = Game::scene->createEntity(id_string + "_leg_right",
                                                           drive.mesh + "_leg_right.mesh");
  Ogre::Entity* leg_left_mesh = Game::scene->createEntity(id_string + "_leg_left",
                                                          drive.mesh + "_leg_left.mesh");
  // assign materials
  drive_mesh->setMaterialName(design.material);
  chasis_mesh->setMaterialName(design.material);
  arm_left_mesh->setMaterialName(design.material);
  arm_right_mesh->setMaterialName(design.material);
  leg_left_mesh->setMaterialName(design.material);
  leg_right_mesh->setMaterialName(design.material);

  // create root node (drive) and attach chasis node as a child
  Ogre::SceneNode* crusader_node = Game::scene->getRootSceneNode()->createChildSceneNode();
  Ogre::SceneNode* chasis_node = crusader_node->createChildSceneNode();
  Ogre::SceneNode* arm_right_node = chasis_node->createChildSceneNode(); // atach arms to chasis
  Ogre::SceneNode* arm_left_node = chasis_node->createChildSceneNode();

  // attach meshes
  crusader_node->attachObject(drive_mesh);
  chasis_node->attachObject(chasis_mesh);
  arm_right_node->attachObject(arm_right_mesh);
  arm_left_node->attachObject(arm_left_mesh);
  crusader_node->attachObject(leg_right_mesh); // legs attached directly to drive
  crusader_node->attachObject(leg_left_mesh);

  // set flags to entities
  drive_mesh->setQueryFlags(query_mask_units);
  chasis_mesh->setQueryFlags(query_mask_units);
  arm_left_mesh->setQueryFlags(query_mask_units);
  arm_right_mesh->setQueryFlags(query_mask_units);
  leg_left_mesh->setQueryFlags(query_mask_units);
  leg_right_mesh->setQueryFlags(query_mask_units);

  // pass the root node (drive) to the crusader object
  Crusader* spawn = new Crusader(a_pos_xyz, chasis.mesh, crusader_node, a_orientation,
                                 design, engine, drive, chasis);

  // put the unit on the list
  units.push_back(spawn);

  return spawn;
}
