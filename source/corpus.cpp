//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "corpus.h"
#include "game_controller.h"
#include "collision.h"
#include "query_mask.h"
#include "game.h"
#include "arena.h"

ulint Corpus::unique_id = 0; //unieque id for each corpus entity in game

Corpus::Corpus(Ogre::Vector3 a_pos_xyz, const string& a_unit_name, Ogre::SceneNode* a_scene_node,
               Ogre::Quaternion a_orientation)
    : scene_node(a_scene_node), bounding_sphere_invalid(true), es_invalid(), cs_invalid(),
    dt(0), pos_xyz(a_pos_xyz), orientation(a_orientation), unit_name(a_unit_name),
    controller_active(false), collision(collision_type_blocking), penetration(0),
    friction(0.5), conductivity(1), cell_index(make_pair(0, 0)), display_collision_debug(false)
{
    surface_temperature = Game::arena->getAmbientTemperature(a_pos_xyz);
    direction = orientation * Ogre::Vector3::UNIT_Z;
    loadCollisionSpheres();

    //make sure it's within bounds
    if (Game::arena->isOutOfBounds(pos_xyz)) {
        Game::kill(unit_name+" is out of bounds, possibly garbled arena definition");
    }
}

/** @brief get bounding sphere and update it's position
  */
inline Sphere Corpus::getBoundingSphere()
{
    if (bounding_sphere_invalid) { //if object moved last frame offset sphere
        bounding_sphere_invalid = false;
        bounding_sphere.centre = relative_bs_position + pos_xyz;
    }

    return bounding_sphere;
}

/** @brief get exclusion spheres based on the passed in sphere
  */
bitset<max_num_es> Corpus::getExclusionSpheres(Sphere& sphere)
{
    //by default return no spheres
    bitset<max_num_es> es_bitset;

    for (usint i = 0, for_size = exclusion_spheres.size(); i < for_size; ++i) {
        //recaltulate position if it's first time this tick
        if (es_invalid[i]) {
            exclusion_spheres[i].sphere.centre = orientation * relative_es_positions[i] + pos_xyz;

            //mark recalculated
            es_invalid[i] = 0;
        }

        //if it intersects return as potential exclusion sphere
        if (exclusion_spheres[i].sphere.intersects(sphere)) {
            es_bitset[i] = 1;
        }
    }

    return es_bitset;
}

/** @brief get collision spheres without excluding any - just update
  */
bitset<max_num_cs> Corpus::getCollisionSpheres(Sphere& sphere)
{
    bitset<max_num_es> es_bitset;

    //assume it intersects with all spheres
    es_bitset.set();

    return getCollisionSpheres(sphere, es_bitset);
}

/** @brief gets collision spheres but doesn't check the ones excluded by es_bitset
  *        and updates their positions based on object position
  */
bitset<max_num_cs> Corpus::getCollisionSpheres(Sphere& sphere, bitset<max_num_es> es_bitset)
{
    bitset<max_num_cs> cs_bitset;

    //by default return all spherers
    cs_bitset.set();

    //stop this loop if you eliminate all spheres
    for (usint i = 0; i < exclusion_spheres.size() && cs_bitset.any(); ++i) {
        //only check potential exclusion spheres
        if (es_bitset[i]) {
            //recaltulate position if it's first time this tick
            if (es_invalid[i]) {
                //mark recalculated
                es_invalid[i] = 0;

                //reaclucalte position depenting on current unit position and orientation
                exclusion_spheres[i].sphere.centre = orientation * relative_es_positions[i]
                                                     + pos_xyz;
            }

            //if it doesn't intersect with the exclusion sphere
            if (exclusion_spheres[i].sphere.intersects(sphere) == false) {
                //exclude the all spheres within the exclusion sphere
                cs_bitset &= exclusion_spheres[i].cs_bitset;
            }
        } else {
            //if it's already been checked to be outside - exclude
            cs_bitset &= exclusion_spheres[i].cs_bitset;
        }
    }

    //check if it's returning anything at all
    if (cs_bitset.any()) {
        //make sure the spheres the indices we return of are in an up to date position
        for (usint i = 0, for_size = collision_spheres.size(); i < for_size; ++i) {
            //recalculate position if it's first time this tick
            if (cs_bitset[i] && cs_invalid[i]) {
                //mark recalculated
                cs_invalid[i] = 0;

                //reaclucalte position depenting on current unit position and orientation
                collision_spheres[i].centre = orientation * relative_cs_positions[i] + pos_xyz;
            }
        }
    }

    return cs_bitset;
}

/** @brief loads collision spheres from a file based on mesh name
  * @todo: load them from a file!
  */
void Corpus::loadCollisionSpheres()
{
    //fake TODO: read from file and partially autocalculate
    if (unit_name == "bullet") {
        relative_bs_position = Ogre::Vector3::ZERO;
        bounding_sphere = Sphere(relative_bs_position, 2);

    } else {
        relative_bs_position = Ogre::Vector3::ZERO;
        bounding_sphere = Sphere(relative_bs_position, 8);
        exclusion_sphere_t exclusion_sphere;

        relative_es_positions.push_back(Ogre::Vector3(-9, 0, 0));
        exclusion_sphere.sphere = Sphere(relative_es_positions.back(), 9);
        exclusion_sphere.cs_bitset = bitset<max_num_cs>(string("110011"));
        exclusion_spheres.push_back(exclusion_sphere);

        relative_es_positions.push_back(Ogre::Vector3(9, 0, 0));
        exclusion_sphere.sphere = Sphere(relative_es_positions.back(), 9);
        exclusion_sphere.cs_bitset = bitset<max_num_cs>(string("11001100"));
        exclusion_spheres.push_back(exclusion_sphere);

        relative_es_positions.push_back(Ogre::Vector3(0, 0, -9));
        exclusion_sphere.sphere = Sphere(relative_es_positions.back(), 9);
        exclusion_sphere.cs_bitset = bitset<max_num_cs>(string("1100110"));
        exclusion_spheres.push_back(exclusion_sphere);

        relative_es_positions.push_back(Ogre::Vector3(0, 0, 9));
        exclusion_sphere.sphere = Sphere(relative_es_positions.back(), 9);
        exclusion_sphere.cs_bitset = bitset<max_num_cs>(string("10011001"));
        exclusion_spheres.push_back(exclusion_sphere);

        relative_cs_positions.push_back(Ogre::Vector3(2, 2, -2));
        collision_spheres.push_back(Sphere(3));
        relative_cs_positions.push_back(Ogre::Vector3(2, 2, 2));
        collision_spheres.push_back(Sphere(3));
        relative_cs_positions.push_back(Ogre::Vector3(-2, 2, 2));
        collision_spheres.push_back(Sphere(3));
        relative_cs_positions.push_back(Ogre::Vector3(-2, 2, -2));
        collision_spheres.push_back(Sphere(3));

        relative_cs_positions.push_back(Ogre::Vector3(2, -2, -2));
        collision_spheres.push_back(Sphere(3));
        relative_cs_positions.push_back(Ogre::Vector3(2, -2, 2));
        collision_spheres.push_back(Sphere(3));
        relative_cs_positions.push_back(Ogre::Vector3(-2, -2, 2));
        collision_spheres.push_back(Sphere(3));
        relative_cs_positions.push_back(Ogre::Vector3(-2, -2, -2));
        collision_spheres.push_back(Sphere(3));

        for (usint i = 0, for_size = collision_spheres.size(); i < for_size; ++i) {
            collision_spheres[i].centre = relative_cs_positions[i];
            cs_areas.push_back(0);
        }

        for (usint i = 0, for_size = exclusion_spheres.size(); i < for_size; ++i) {
            cs_areas.push_back(0);
        }
    }
    //displayCollision(true);
}

/** @brief get a string with a unique id for Ogre
  */
string Corpus::getUniqueID()
{
    string id_string;
    stringstream stream;
    stream << setfill('0') << setw(10) << unique_id++; //format id string to 0000000001
    stream >> id_string;
    return id_string;
}

/** @brief debug spheres create and destroy
  * not safe to call false unless you call true first
  */
void Corpus::displayCollision(bool a_toggle)
{
    display_collision_debug = a_toggle;

    if (display_collision_debug) {
        string id_string = getUniqueID();
        //bounding sphere
        debug_bs_node = Game::scene->getRootSceneNode()->createChildSceneNode();
        debug_bs_entity = Game::scene->createEntity(id_string+"_debug_sphere",
                          "sphere2.mesh");
        debug_bs_entity->setCastShadows(false);

        //attach meshes
        debug_bs_node->attachObject(debug_bs_entity);
        debug_bs_entity->setQueryFlags(query_mask_ignore);
        debug_bs_node->setScale(bounding_sphere.radius, bounding_sphere.radius,
                                bounding_sphere.radius);

        for (usint i = 0, for_size = collision_spheres.size(); i < for_size; ++i) {
            id_string = getUniqueID();
            debug_cs_nodes.push_back(Game::scene->getRootSceneNode()->createChildSceneNode());
            debug_cs_entities.push_back(Game::scene->createEntity(id_string+"_debug_sphere",
                                        "sphere.mesh"));
            debug_cs_entities.back()->setCastShadows(false);
            debug_cs_nodes.back()->attachObject(debug_cs_entities.back()); //attach meshes
            debug_cs_entities.back()->setQueryFlags(query_mask_ignore);
            debug_cs_nodes.back()->setScale(collision_spheres.at(0).radius,
                                            collision_spheres.at(0).radius,
                                            collision_spheres.at(0).radius);
        }
    } else {
        //destroy attached entities
        debug_bs_node->getCreator()->destroyMovableObject(debug_bs_entity);
        Game::scene->destroySceneNode(debug_bs_node);

        for (usint i = 0, for_size = collision_spheres.size(); i < for_size; ++i) {
            debug_cs_nodes.back()->getCreator()->destroyMovableObject(debug_cs_entities[i]);
            Game::scene->destroySceneNode(debug_cs_nodes[i]);
        }

        debug_cs_nodes.clear();
        debug_cs_entities.clear();
    }
}

/** @brief debug spheres update to current positions
  * as above - don't use unless spheres exist
  */
void Corpus::displayCollisionUpdate()
{
    debug_bs_node->setPosition(bounding_sphere.centre);
    for (usint i = 0, for_size = collision_spheres.size(); i < for_size; ++i) {
        debug_cs_nodes[i]->setPosition(collision_spheres[i].centre);
    }
}

Ogre::Vector3 Corpus::getDirection()
{
    return direction;
}

/** @brief ignores collisions
  * @todo: use this to react graphically to a collision
  */
int Corpus::handleCollision(Collision* a_collision)
{
    return 0;
}

/** @brief game logic, physics and control
  */
int Corpus::update(Ogre::Real a_dt)
{
    int update_return;

    dt = a_dt;

    //temp debug
    if (display_collision_debug) {
        displayCollisionUpdate();
    }

    if (controller_active) {
        update_return = updateController();
    } else {
        update_return = 2;
    }

    //temp debug
    if (update_return == 1 && display_collision_debug) {
        displayCollision(false);
    }

    return update_return;
}

/** @brief starts polling the controller or AI for input
  */
inline void Corpus::activateController(bool a_toggle)
{
    controller_active = a_toggle;
}

/** @brief assign a controller which can be boung to an input handler or an AI
  */
void Corpus::assignController(GameController* a_controller)
{
    controller = a_controller;
    controller_active = true;
}

void Corpus::removeController()
{
    controller = 0;
    controller_active = false;
}
