//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "arena.h"
#include "game.h"
#include "query_mask.h"
#include "corpus_factory.h"
#include "unit_factory.h"
#include "projectile_factory.h"
#include "particle_factory.h"
#include "formation_factory.h"
#include "ai_factory.h"
#include "crusader_ai.h"
#include "crusader.h"
#include "files_handler.h"
#include "collision_handler.h"
#include "camera.h"
#include "game_controller.h"
#include "hud.h"
#include "faction.h"
#include "formation.h"

//1pixel = 8 metres
const Ogre::Real metres_per_pixel = 8;
//cell size
const int real_page_size = 256; //in metres
const usint page_size = real_page_size / metres_per_pixel; //in pixels
//white colour means 16 * scale metres high
const Ogre::Real max_height = 16 * metres_per_pixel; //128

const string terrain_material_name = "terrain_material";

Arena::Arena()
{
    //objects only needed when in arena but using game for global scoping
    Game::ai_factory = new AIFactory();
    Game::corpus_factory = new CorpusFactory();
    Game::projectile_factory = new ProjectileFactory();
    Game::unit_factory = new UnitFactory();
    Game::particle_factory = new ParticleFactory();
    Game::hud = new Hud();
    Game::collider = new CollisionHandler();

    //create lights
    sunlight = Game::scene->createLight("sunlight");
    backlight = Game::scene->createLight("backlight");
    ground_light = Game::scene->createLight("ground_light");

    //no ambient light - we use ground light to fake that
    Game::scene->setAmbientLight(Ogre::ColourValue::Black);
    //this could possibly be moved to update light
    Game::scene->setShadowColour(Ogre::ColourValue(0.3, 0.3, 0.4));

    //sunlight casting shadows
    sunlight->setType(Ogre::Light::LT_DIRECTIONAL);
    sunlight->setPosition(Ogre::Vector3(100, 200, -100)); //TODO: change to spot for texture shadows
    sunlight->setCastShadows(true);
    backlight->setType(Ogre::Light::LT_DIRECTIONAL);
    backlight->setCastShadows(false);
    ground_light->setType(Ogre::Light::LT_DIRECTIONAL);
    ground_light->setCastShadows(false);
}

Ogre::Real Arena::getAmbientTemperature(Ogre::Vector3 a_position)
{
    //temp
    return 20;
}

Arena::~Arena()
{
    //cleanup ai controllers
    for (unsigned int i = 0, for_size = ai_game_controllers.size(); i < for_size; ++i) {
        delete ai_game_controllers[i];
    }

    //cleanup lights
    Game::scene->destroyAllLights();

    //destroy terrain mesh
    CorpusFactory::destroyModel(terrain_node);

    //cleanup terrain
    Ogre::MaterialManager::getSingleton().remove(terrain_material_name);

    //cleanup game objects created for the arena
    delete Game::ai_factory;
    delete Game::corpus_factory;
    delete Game::projectile_factory;
    delete Game::unit_factory;
    delete Game::particle_factory;
    delete Game::hud;
    delete Game::collider;
}

/** @brief changes light according to time, date, weather
  * @todo: update lights based on world conditions and do it at 1pfs
  */
void Arena::updateLights()
{
    //z+ is left //x- is forward //y+ is up
    //read those from a table witch hashes time, date, location, weather
    Ogre::Real r = 0.9;
    Ogre::Real g = 0.8;
    Ogre::Real b = 0.7;

    //sunlight
    sunlight->setDiffuseColour(r, g, b);
    sunlight->setDirection(-0.5, -1, 0.5);

    //backlight
    backlight->setDiffuseColour(b * 0.25, g * 0.25, r * 0.5);
    backlight->setDirection(1, -0.5, -1);

    //fake ambient TODO: take dominant ground colour
    ground_light->setDiffuseColour(0.8, 0.5, 0.4);
    ground_light->setDirection(0.1, 1, 0.3);
}

/** @brief loads a map when you enter a location and populates it with stuff
  * @todo: loadArena from a script file and move this to a class
  */
int Arena::loadArena(const string& arena_name)
{
    //TODO: read form file obviously
    gravity = 10; //m/s^2

    terrain_name = "mars";

    //load the terrain from files
    loadTerrain();

    //prepare the cells
    partitionArena();

    //TODO: read faction from game state file (save file) outside of the arena
    //factions
    Faction* faction_imperium = new Faction(global_faction::imperium);
    Faction* faction_mercenary = new Faction(global_faction::mercenary);
    //formations
    Formation* enemy_formation = Game::formation_factory->createFormation("enemies",
                                                                          faction_imperium);
    Formation* player_formation = Game::formation_factory->createFormation("players",
                                                                           faction_mercenary);
    Formation* allied_formation = Game::formation_factory->createFormation("allies",
                                                                           faction_mercenary);

    //fake game startup from code - ought to be read from file
    Crusader* player_unit = Game::unit_factory->spawnCrusader(Ogre::Vector3(600, 0, 600),
                            "base_husar_cavalry");
    //set player unit controller to local input
    player_unit->assignController(Game::instance()->getGameController(0));
    player_formation->joinFormation(Game::instance()->getGameController(0));

    //create the hud according to the unit you're in - HUD NEEDS THE CONTROLLER to be assigned!
    Game::hud->loadHud(static_cast<Unit*>(player_unit));

    Game::corpus_factory->spawnSceneryBuidling(Ogre::Vector3(630, getHeight(630, 650), 650),
                                               "building_test_01");

    Game::corpus_factory->spawnSceneryBuidling(Ogre::Vector3(570, getHeight(630, 650), 650),
                                               "building_test_02");

//    //create enemies
//    Crusader* enemy_unit2 = Game::unit_factory->spawnCrusader(Ogre::Vector3(780, 0, 680),
//                           "base_husar_cavalry_red");
//    Crusader* enemy_unit = Game::unit_factory->spawnCrusader(Ogre::Vector3(600, 0, 700),
//                            "base_husar_cavalry_red");
//
//
//    //create an enemy controller
//    ai_game_controllers.push_back(new GameController("dummy ai"));
//    CrusaderAI* ai = Game::ai_factory->createCrusaderAI(enemy_unit);
//    ai->bindController(ai_game_controllers.back()); //assign the controller to the ai
//    ai->activate(false);
//    enemy_unit->assignController(ai_game_controllers.back()); //give the unit to the ai controller
//    enemy_formation->joinFormation(ai_game_controllers.back());
//
//    //second enemy
//    ai_game_controllers.push_back(new GameController("dummy ai"));
//    ai = Game::ai_factory->createCrusaderAI(enemy_unit2);
//    ai->bindController(ai_game_controllers.back()); //assign the controller to the ai
//    ai->activate(true);
//    enemy_unit2->assignController(ai_game_controllers.back()); //give the unit to the ai controller
//    allied_formation->joinFormation(ai_game_controllers.back());

    //and tell the camera to follow the players unit
    Game::camera->follow(player_unit);

    //create the terrain
    createTerrain();

    //lights!
    updateLights();

    return 0;
}

void Arena::partitionArena()
{
    //set size for the cells holding the object on the map
    corpus_cells = vector<vector<list<Corpus*> > >(num_of_arena_cells,
                                                   vector<list<Corpus*> >(num_of_arena_cells));
    mobilis_cells = vector<vector<list<Mobilis*> > >(num_of_arena_cells,
                                                     vector<list<Mobilis*> >(num_of_arena_cells));
    unit_cells = vector<vector<list<Unit*> > >(num_of_arena_cells,
                                               vector<list<Unit*> >(num_of_arena_cells));
}

/** @brief loads the terrain
  * @todo: load a terrain definition form file
  */
void Arena::loadTerrain()
{
    //load the heightmap
    Ogre::Image* heightmap = new Ogre::Image();
    heightmap->load(terrain_name+".tga", "terrain");

    //precompute scales and sizes
    texture_size = heightmap->getWidth();
    inverse_scale = Ogre::Real(1) / metres_per_pixel;

    //size of the map in metres
    scene_size = texture_size * metres_per_pixel;

    //number of cells with objects inside them
    num_of_arena_cells = scene_size / size_of_arena_cell;

    //heightmap data
    height = new Ogre::Real[texture_size * texture_size];
    angle_x = new Ogre::Real[texture_size * texture_size];
    angle_y = new Ogre::Real[texture_size * texture_size];

    //load the data from the image
    for (unsigned int i = 0; i < texture_size; ++i) {
        for (unsigned int j = 0; j < texture_size; ++j) {
            Ogre::ColourValue colour = heightmap->getColourAt(i, j, 0);
            height[i + j * texture_size] = colour.a * max_height;
            angle_x[i + j * texture_size] = colour.r;
            angle_y[i + j * texture_size] = colour.g;
        }
    }

    delete heightmap;
}

/** @brief creates the terrain file
  * @todo: stop rereading the files and do this as a pixel shader on a flat polygon
  */
void Arena::createTerrain()
{
    //detail texture mapping
    const Ogre::Real detail_density = 16;

    //mesh density
    const usint lod_bias = 0;
    const usint step = 1 << lod_bias;

    //calculate number of terrain pages
    const usint num_of_pages = texture_size / page_size;

    //keep this as reading the image for now until I move to voxels (hopefully)
    Ogre::Image* heightmap = new Ogre::Image();
    heightmap->load(terrain_name+".tga", "terrain");
    Ogre::Image* detail_mask = new Ogre::Image();
    detail_mask->load(terrain_name+"_detail_mask.tga", "terrain");

    //node to hold all terrain cells
    terrain_node = Game::scene->getRootSceneNode()->createChildSceneNode();

    //material for the terrain
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(terrain_material_name,
                                                                              "terrain");

    //single pass multitexture material (mutlipass created automatically, thanks Ogre)
    Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
    //use vertex colour for diffuse
    pass->setVertexColourTracking(Ogre::TVC_DIFFUSE);

    //base detail texture 0
    pass->createTextureUnitState()->setTextureName("sand_01.dds");
    pass->getTextureUnitState(0)->setTextureCoordSet(1);
    pass->getTextureUnitState(0)->setColourOperationEx(Ogre::LBX_SOURCE1,
                                                       Ogre::LBS_TEXTURE,
                                                       Ogre::LBS_CURRENT);

    //detail texture 1
    pass->createTextureUnitState()->setTextureName("rock_01.dds");
    pass->getTextureUnitState(1)->setTextureCoordSet(1);
    pass->getTextureUnitState(1)->setColourOperationEx(Ogre::LBX_BLEND_DIFFUSE_ALPHA,
                                                       Ogre::LBS_TEXTURE,
                                                       Ogre::LBS_CURRENT);

    //detail texture 2
    pass->createTextureUnitState()->setTextureName("dirt_01.dds");
    pass->getTextureUnitState(2)->setTextureCoordSet(1);
    pass->getTextureUnitState(2)->setColourOperationEx(Ogre::LBX_BLEND_DIFFUSE_COLOUR,
                                                       Ogre::LBS_TEXTURE,
                                                       Ogre::LBS_CURRENT);

    //colour texture
    pass->createTextureUnitState()->setTextureName(terrain_name+"_colour.dds");
    pass->getTextureUnitState(3)->setTextureCoordSet(0);
    //modulate colour and detail
    pass->getTextureUnitState(3)->setColourOperation(Ogre::LBO_MODULATE);

    //keep this as reading the image for now until I move to voxels (hopefully)

    //create cells containing portions of the terrain so it can be culled when not on screen
    for (usint k = 0; k < num_of_pages; ++k) {
        for (usint l = 0; l < num_of_pages; ++l) {
            //offsets for this cell
            unsigned int origin_x = k * page_size;
            unsigned int origin_y = l * page_size;

            //create the mesh part
            Ogre::ManualObject* terrain_mesh = Game::scene->createManualObject(
                                                            "terrain_mesh_"+Game::intIntoString(l)
                                                            +"_" +Game::intIntoString(k));

            //create the mesh manually by defining triangle pairs - apply the terrain_name material
            terrain_mesh->begin(terrain_material_name);

            for (unsigned int i = 0; i + step <= page_size; i += step) {
                unsigned int is = i + step;
                for (unsigned int j = 0; j + step <= page_size; j += step) {
                    unsigned int js = j + step;

                    //sample the heightmap to get elevation and normals
                    Ogre::ColourValue colour;
                    Ogre::ColourValue detail_colour;

                    //first bottom-left vertex
                    colour = heightmap->getColourAt((origin_x + i) % texture_size,
                                                    (origin_y + j) % texture_size, 0);
                    detail_colour = detail_mask->getColourAt((origin_x + i) % texture_size,
                                                             (origin_y + j) % texture_size, 0);
                    terrain_mesh->position(i * metres_per_pixel, (colour.a) * max_height,
                                           j * metres_per_pixel);
                    terrain_mesh->textureCoord((origin_x + i) / Ogre::Real(texture_size),
                                               (origin_y + j) / Ogre::Real(texture_size));
                    terrain_mesh->textureCoord(i / detail_density, j / detail_density);
                    terrain_mesh->normal(colour.r, colour.g, 0.5);
                    terrain_mesh->colour(detail_colour);

                    //first bottom-right vertex
                    colour = heightmap->getColourAt((origin_x + i) % texture_size,
                                                    (origin_y + js) % texture_size, 0);
                    detail_colour = detail_mask->getColourAt((origin_x + i) % texture_size,
                                                             (origin_y + js) % texture_size, 0);
                    terrain_mesh->position(i * metres_per_pixel, (colour.a) * max_height,
                                           js * metres_per_pixel);
                    terrain_mesh->textureCoord((origin_x + i) / Ogre::Real(texture_size),
                                               (origin_y + js) / Ogre::Real(texture_size));
                    terrain_mesh->textureCoord(i / detail_density, js / detail_density);
                    terrain_mesh->normal(colour.r, colour.g, 0.5);
                    terrain_mesh->colour(detail_colour);

                    //first top-right vertex
                    colour = heightmap->getColourAt((origin_x + is) % texture_size,
                                                    (origin_y + js) % texture_size, 0);
                    detail_colour = detail_mask->getColourAt((origin_x + is) % texture_size,
                                                             (origin_y + js) % texture_size, 0);
                    terrain_mesh->position(is * metres_per_pixel, (colour.a) * max_height,
                                           js * metres_per_pixel);
                    terrain_mesh->textureCoord((origin_x + is) / Ogre::Real(texture_size),
                                               (origin_y + js) / Ogre::Real(texture_size));
                    terrain_mesh->textureCoord(is / detail_density, js / detail_density);
                    terrain_mesh->normal(colour.r, colour.g, 0.5);
                    terrain_mesh->colour(detail_colour);

                    //second top-left vertex
                    colour = heightmap->getColourAt((origin_x + is) % texture_size,
                                                    (origin_y + j) % texture_size, 0);
                    detail_colour = detail_mask->getColourAt((origin_x + is) % texture_size,
                                                             (origin_y + j) % texture_size, 0);
                    terrain_mesh->position(is * metres_per_pixel, (colour.a) * max_height,
                                           j * metres_per_pixel);
                    terrain_mesh->textureCoord((origin_x + is) / Ogre::Real(texture_size),
                                               (origin_y + j) / Ogre::Real(texture_size));
                    terrain_mesh->textureCoord(is / detail_density, j / detail_density);
                    terrain_mesh->normal(colour.r, colour.g, 0.5);
                    terrain_mesh->colour(detail_colour);

                    //second bottom-left vertex
                    colour = heightmap->getColourAt((origin_x + i) % texture_size,
                                                    (origin_y + j) % texture_size, 0);
                    detail_colour = detail_mask->getColourAt((origin_x + i) % texture_size,
                                                             (origin_y + j) % texture_size, 0);
                    terrain_mesh->position(i * metres_per_pixel, (colour.a) * max_height,
                                           j * metres_per_pixel);
                    terrain_mesh->textureCoord((origin_x + i) / Ogre::Real(texture_size),
                                               (origin_y + j) / Ogre::Real(texture_size));
                    terrain_mesh->textureCoord(i / detail_density, j / detail_density);
                    terrain_mesh->normal(colour.r, colour.g, 0.5);
                    terrain_mesh->colour(detail_colour);

                    //second top-right vertex
                    colour = heightmap->getColourAt((origin_x + is) % texture_size,
                                                    (origin_y + js) % texture_size, 0);
                    detail_colour = detail_mask->getColourAt((origin_x + is) % texture_size,
                                                             (origin_y + js) % texture_size, 0);
                    terrain_mesh->position(is * metres_per_pixel, (colour.a) * max_height,
                                           js * metres_per_pixel);
                    terrain_mesh->textureCoord((origin_x + is) / Ogre::Real(texture_size),
                                               (origin_y + js) / Ogre::Real(texture_size));
                    terrain_mesh->textureCoord(is / detail_density, js / detail_density);
                    terrain_mesh->normal(colour.r, colour.g, 0.5);
                    terrain_mesh->colour(detail_colour);
                }
            }

            //finished creating traignles
            terrain_mesh->end();

            //terrain entity
            terrain_mesh->setQueryFlags(query_mask_ignore);
            terrain_mesh->setCastShadows(false);

            //attach to a scene node
            Ogre::SceneNode* cell_node = terrain_node->createChildSceneNode();
            cell_node->attachObject(terrain_mesh);

            //position cells in agrid
            cell_node->setPosition(origin_x * metres_per_pixel, 0,
                                   origin_y * metres_per_pixel);
        }
    }

    delete heightmap;
    delete detail_mask;
}

/** @brief returns the height of the terrain at this point
  */
Ogre::Real Arena::getHeight(Ogre::Real a_x, Ogre::Real a_y)
{
    //translate world coords into texture coords
    Ogre::Real texture_x = a_x * inverse_scale;
    Ogre::Real texture_y = a_y * inverse_scale;

    //get upper left corner index of of the sampled square
    unsigned int i = texture_x;
    unsigned int j = texture_y;

    //sample four heights
    Ogre::Real sample1 = height[i + j * texture_size];
    Ogre::Real sample2 = height[i + 1 + j * texture_size];
    Ogre::Real sample3 = height[i + (j + 1) * texture_size];
    Ogre::Real sample4 = height[i + 1 + (j + 1) * texture_size];

    //get x and y in the 1 by 1 pixel size cell
    Ogre::Real weight_x = texture_x - i;
    Ogre::Real weight_y = texture_y - j;
    Ogre::Real weight_x_co1 = 1 - weight_x;
    Ogre::Real weight_y_co1 = 1 - weight_y;

    //calculate weights for each sample
    Ogre::Real weight1 = weight_x_co1 * weight_y_co1;
    Ogre::Real weight2 = weight_x * weight_y_co1;
    Ogre::Real weight3 = weight_x_co1 * weight_y;
    Ogre::Real weight4 = weight_x * weight_y;

    //return the sum of weighted samples
    return weight1 * sample1 + weight2 * sample2 + weight3 * sample3 + weight4 * sample4;
}

/** @brief fills the vector with indices of cells within the given radius from a cell
  * you could cut off the corner indices for a big radius but it's hardly worth the bother
  * at radius 4 you could save one cell, OK, TODO: cut them off
  */
void Arena::getCellIndicesWithinRadius(const uint_pair a_index, vector<uint_pair>& indices,
                                       const Ogre::Real a_radius)
{
    //round up and add half the cell size to get possible hits from without the radius
    int cell_radius = (a_radius + size_of_arena_cell * 0.5) / size_of_arena_cell + 1;

    //push the centre cell
    indices.push_back(a_index);

    //centre for the spiral (measure from the edge, so even radius 0 gets 1 spiral arm)
    uint c_i = a_index.first;
    uint c_j = a_index.second;

    //this goes in a spriral way putting in indices closer to first and further away last
    //every time it checks if the index is valid
    for (int r = 1; r <= cell_radius; ++r) {
        if (c_i + r >= 0 && c_i + r <= num_of_arena_cells) {
            for (int i = -r; i <= r; ++i) {
                if (c_j + i >= 0 && c_j + i <= num_of_arena_cells) {
                    indices.push_back(make_pair(c_i + r, c_j + i));
                }
            }
        }
        if (c_i - r >= 0 && c_i - r <= num_of_arena_cells) {
            for (int i = -r; i <= r; ++i) {
                if (c_j + i >= 0 && c_j + i <= num_of_arena_cells) {
                    indices.push_back(make_pair(c_i - r, c_j + i));
                }
            }
        }
        if (c_j + r >= 0 && c_j + r <= num_of_arena_cells) {
            for (int i = -(r - 1); i <= (r - 1); ++i) {
                if (c_i + i >= 0 && c_i + i <= num_of_arena_cells) {
                    indices.push_back(make_pair(c_i + i, c_j + r));
                }
            }
        }
        if (c_j - r >= 0 && c_j - r <= num_of_arena_cells) {
            for (int i = -(r - 1); i <= (r - 1); ++i) {
                if (c_i + i >= 0 && c_i + i <= num_of_arena_cells) {
                    indices.push_back(make_pair(c_i + i, c_j - r));
                }
            }
        }
    }
}

/** @brief called when the object is newly created to add it to the cell (saves on checking)
  */
void Arena::setCellIndex(uint_pair a_cell_index, Corpus* a_thing)
{
    //add to the cell and leave it there, corpus does not move
    corpus_cells[a_cell_index.first][a_cell_index.second].push_back(a_thing);
}

/** @brief moves the object's pointer to the new cell if needed
  */
bool Arena::updateCellIndex(uint_pair& cell_index, Ogre::Vector3& pos_xyz, Mobilis* a_thing)
{
    //if it's not within the limits of the arena rectiify the position
    bool out_of_bounds = isOutOfBounds(pos_xyz);
    //get the cell index based on position
    uint_pair new_cell_index = getCellIndex(pos_xyz.x, pos_xyz.z);

    if (cell_index != new_cell_index) {
        //remove the old cell
        mobilis_cells[cell_index.first][cell_index.second].remove(a_thing);

        //if no objects left remove from the list of live cells
        if (mobilis_cells[cell_index.first][cell_index.second].size() == 0) {
            live_mobilis_cells.remove(&mobilis_cells[cell_index.first][cell_index.second]);
        }

        //set the new index
        cell_index = new_cell_index;

        //add to the new cell
        mobilis_cells[cell_index.first][cell_index.second].push_back(a_thing);

        //if it's the first object inset the cell into the list of live cells
        if (mobilis_cells[cell_index.first][cell_index.second].size() == 1) {
            live_mobilis_cells.push_back(&mobilis_cells[cell_index.first][cell_index.second]);
        }
    }

    return out_of_bounds;
}

/** @brief moves the object's pointer to the new cell if needed
  * same function different array (unit_cells)
  */
bool Arena::updateCellIndex(uint_pair& cell_index, Ogre::Vector3& pos_xyz, Unit* a_thing)
{
    //if it's not within the limits of the arena rectiify the position
    bool out_of_bounds = isOutOfBounds(pos_xyz);
    //get the cell index based on position
    uint_pair new_cell_index = getCellIndex(pos_xyz.x, pos_xyz.z);

    if (cell_index != new_cell_index) {
        //remove the old cell
        unit_cells[cell_index.first][cell_index.second].remove(a_thing);

        //if no objects left remove from the list of live cells
        if (unit_cells[cell_index.first][cell_index.second].size() == 0) {
            live_unit_cells.remove(&unit_cells[cell_index.first][cell_index.second]);
        }

        //set the new index
        cell_index = new_cell_index;

        //add to the new cell
        unit_cells[cell_index.first][cell_index.second].push_back(a_thing);

        //if it's the first object inset the cell into the list of live cells
        if (unit_cells[cell_index.first][cell_index.second].size() == 1) {
            live_unit_cells.push_back(&unit_cells[cell_index.first][cell_index.second]);
        }
    }

    return out_of_bounds;
}
