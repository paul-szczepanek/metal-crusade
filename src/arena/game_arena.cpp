// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game_arena.h"
#include "game.h"
#include "query_mask.h"
#include "formation_manager.h"
#include "faction_manager.h"
#include "files_handler.h"
#include "game_camera.h"
#include "game_controller.h"
#include "faction.h"
#include "formation.h"
#include "terrain.h"
#include "terrain_generator.h"
#include "building_factory.h"
#include "unit_factory.h"
#include "projectile_factory.h"
#include "particle_manager.h"
#include "ai_manager.h"
#include "crusader_ai.h"
#include "crusader.h"
#include "collision_handler.h"
#include "nav_point.h"

/*
 #include "hud.h"*/

// cell size
const int real_page_size = 256; // in metres
const usint page_size = real_page_size / metres_per_pixel; // in pixels
// white colour means 16 * scale metres high
const Real max_height = 16 * metres_per_pixel; // 128

const string terrain_material_name = "terrain_material";

GameArena::GameArena()
{
  // create lights
  sunlight = Game::Scene->createLight("sunlight");
  backlight = Game::Scene->createLight("backlight");
  ground_light = Game::Scene->createLight("ground_light");

  // no ambient light - we use ground light to fake that
  Game::Scene->setAmbientLight(Ogre::ColourValue::Black);
  // this could possibly be moved to update light
  Game::Scene->setShadowColour(Ogre::ColourValue(0.3, 0.3, 0.4));

  // sunlight casting shadows
  sunlight->setType(Ogre::Light::LT_DIRECTIONAL);
  sunlight->setPosition(Vector3(100, 200, -100)); // TODO: change to spot for texture shadows
  sunlight->setCastShadows(true);
  backlight->setType(Ogre::Light::LT_DIRECTIONAL);
  backlight->setCastShadows(false);
  ground_light->setType(Ogre::Light::LT_DIRECTIONAL);
  ground_light->setCastShadows(false);
}

Real GameArena::getAmbientTemperature(Vector3 a_position)
{
  // temp
  return 20;
}

bool GameArena::update(const Real a_dt)
{
  for (auto it : Entities) {
    it->update(a_dt);
  }
  return true;
}

GameArena::~GameArena()
{
  // cleanup lights
  Game::Scene->destroyAllLights();

  // destroy terrain mesh
  Game::destroyModel(terrain_node);

  // cleanup terrain
  Ogre::MaterialManager::getSingleton().remove(terrain_material_name);

  Entities.clear();
}

/** @brief changes light according to time, date, weather
 * @todo: update lights based on world conditions and do it at 1pfs
 */
void GameArena::updateLights()
{
  // z+ is left // x- is forward // y+ is up
  // read those from a table witch hashes time, date, location, weather
  Real r = 0.9;
  Real g = 0.8;
  Real b = 0.7;

  // sunlight
  sunlight->setDiffuseColour(r, g, b);
  sunlight->setDirection(-0.5, -1, 0.5);

  // backlight
  backlight->setDiffuseColour(b * 0.25, g * 0.25, r * 0.5);
  backlight->setDirection(1, -0.5, -1);

  // fake ambient TODO: take dominant ground colour
  ground_light->setDiffuseColour(0.8, 0.5, 0.4);
  ground_light->setDirection(0.1, 1, 0.3);
}

/** @brief returns the height of the arena at this point
 */
Real GameArena::getHeight(Real a_x,
                          Real a_y)
{
  Real terrain_height = terrain->getHeight(a_x, a_y);

  // currently you can only walk on terrain but in the future this is where extra checks will
  // allow to walk on structures like bridges

  return terrain_height;
}

/** @brief loads a map when you enter a location and populates it with stuff
 * @todo: loadArena from a script file and move this to a class
 */
int GameArena::loadArena(const string& arena_name)
{
  // TODO: read from file obviously
  gravity = 10; // m/s^2

  // create the terrain
  terrain = TerrainGenerator::generateTerrain("test");

  // todo: this might go the other way around, so that the terrain adapts to the arena size
  // size of the map in metres
  scene_size_w = terrain->size_w * metres_per_pixel;
  scene_size_h = terrain->size_h * metres_per_pixel;

  // prepare the cells
  partitionArena();

  // TODO: read faction from game state file (save file) outside of the arena
  // factions
  Faction* faction_imperium =
    Game::Faction->createFaction("Imperium", global_faction::imperium);
  Faction* faction_mercenary
    = Game::Faction->createFaction("Mercenary", global_faction::mercenary);
  // formations
  Formation* enemy_formation = Game::Formation->createFormation("enemies",
                                                                faction_imperium);
  Formation* player_formation = Game::Formation->createFormation("players",
                                                                 faction_mercenary);
  Formation* allied_formation = Game::Formation->createFormation("allies",
                                                                 faction_mercenary);

  // temp buildings
  Game::Building->spawnSceneryBuidling(120, 280, "building_test_01");
  Game::Building->spawnSceneryBuidling(280, 300, "building_test_02");
  Game::Building->spawnSceneryBuidling(350, 270, "building_test_02");
  Game::Building->spawnSceneryBuidling(310, 380, "building_test_01");
  Game::Building->spawnSceneryBuidling(210, 240, "building_test_02");
  Game::Building->spawnSceneryBuidling(300, 260, "building_test_02");

  Game::Building->spawnSceneryBuidling(320, 480, "building_test_01");
  Game::Building->spawnSceneryBuidling(880, 300, "building_test_02");
  Game::Building->spawnSceneryBuidling(950, 270, "building_test_02");
  Game::Building->spawnSceneryBuidling(310, 380, "building_test_01");
  Game::Building->spawnSceneryBuidling(110, 340, "building_test_02");
  Game::Building->spawnSceneryBuidling(300, 160, "building_test_02");

  // fake game startup from code - ought to be read from file
  Crusader* player_unit = Game::Unit->spawnCrusader(Vector3(380, 0, 380), "base_husar_cavalry");

  player_unit->assignController(Game::getGameController(0));

  // set player unit controller to local input
  player_formation->joinFormation(player_unit);

  // create the hud according to the unit you're in - HUD NEEDS THE CONTROLLER to be assigned!
  //Game::hud->loadHud(static_cast<Unit*>(player_unit));

  // create ally
  Crusader* ally_unit = Game::Unit->spawnCrusader(Vector3(340, 0, 300),
                                                  "base_husar_cavalry");

  NavPoint* nav_point_a = new NavPoint(Vector3(3600, 0, 3600));

  // give it an ai and have to go to navpoint a
  CrusaderAI* ai = Game::AI->activateUnit(ally_unit, allied_formation);
  ai->setGoal(nav_point_a);

  // create crusaders for enemies
  Crusader* enemy_unit1 = Game::Unit->spawnCrusader(Vector3(200, 0, 100),
                                                    "base_husar_cavalry_red");
  Crusader* enemy_unit2 = Game::Unit->spawnCrusader(Vector3(500, 0, 250),
                                                    "base_husar_cavalry_red");
  Crusader* enemy_unit3 = Game::Unit->spawnCrusader(Vector3(100, 0, 650),
                                                    "base_husar_cavalry_red");
  Crusader* enemy_unit4 = Game::Unit->spawnCrusader(Vector3(300, 0, 250),
                                                    "base_husar_cavalry_red");
  Crusader* enemy_unit5 = Game::Unit->spawnCrusader(Vector3(400, 0, 230),
                                                    "base_husar_cavalry_red");
  Crusader* enemy_unit6 = Game::Unit->spawnCrusader(Vector3(100, 0, 100),
                                                    "base_husar_cavalry_red");

  // create enemies
  Game::AI->activateUnit(enemy_unit1, enemy_formation)->setEnemy(player_unit);
  Game::AI->activateUnit(enemy_unit2, enemy_formation)->setEnemy(ally_unit);
  Game::AI->activateUnit(enemy_unit3, enemy_formation)->setEnemy(player_unit);
  Game::AI->activateUnit(enemy_unit4, enemy_formation)->setEnemy(ally_unit);
  Game::AI->activateUnit(enemy_unit5, enemy_formation)->setEnemy(player_unit);
  Game::AI->activateUnit(enemy_unit6, enemy_formation)->setEnemy(ally_unit);

  // and tell the camera to follow the players unit
  Game::Camera->follow(player_unit);

  createTerrainModel();

  // lights!
  updateLights();

  return 0;
}

/** @brief creates buckets for objects on the map
 */
void GameArena::partitionArena()
{
  // number of cells with objects inside them
  num_of_arena_cells_w = scene_size_w / size_of_arena_cell;
  num_of_arena_cells_h = scene_size_h / size_of_arena_cell;

  // set size for the cells holding the object on the map
  corpus_cells = vector<vector<list<Corpus*> > >(num_of_arena_cells_w,
                                                 vector<list<Corpus*> >(num_of_arena_cells_h));
}

/** @brief creates the mesh for the terrain
 */
void GameArena::createTerrainModel()
{
  // detail texture mapping
  const Real detail_density = 16;

  // mesh density
  const usint lod_bias = 0;
  const usint step = 1 << lod_bias;

  const size_t texture_size_w = terrain->size_w;
  const size_t texture_size_h = terrain->size_h;

  // calculate number of terrain pages
  const usint num_of_pages_w = texture_size_w / page_size;
  const usint num_of_pages_h = texture_size_h / page_size;

  // node to hold all terrain cells
  terrain_node = Game::Scene->getRootSceneNode()->createChildSceneNode();

  // material for the terrain
  Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(terrain_material_name,
                                                                            "terrain");

  // single pass multitexture material (mutlipass created automatically, thanks Ogre)
  Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
  // use vertex colour for diffuse
  pass->setVertexColourTracking(Ogre::TVC_DIFFUSE);

  // base detail texture 0
  pass->createTextureUnitState()->setTextureName("test_grid_128.dds");
  pass->getTextureUnitState(0)->setTextureCoordSet(1);
  pass->getTextureUnitState(0)->setColourOperationEx(Ogre::LBX_SOURCE1,
                                                     Ogre::LBS_TEXTURE,
                                                     Ogre::LBS_CURRENT);

  // colour texture
  // pass->createTextureUnitState()->setTextureName(terrain_name+"_colour.tga");
  // pass->getTextureUnitState(1)->setTextureCoordSet(0);
  // modulate colour and detail
  // pass->getTextureUnitState(1)->setColourOperation(Ogre::LBO_MODULATE);

  // create cells containing portions of the terrain so it can be culled when not on screen
  for (usint k = 0; k < num_of_pages_w; ++k) {
    for (usint l = 0; l < num_of_pages_h; ++l) {
      // offsets for this cell
      size_t origin_x = k * page_size;
      size_t origin_y = l * page_size;

      // create the mesh part
      Ogre::ManualObject* terrain_mesh = Game::Scene->createManualObject(
        "terrain_mesh_" + intoString(l)
        + "_" + intoString(k));

      // create the mesh manually by defining triangle pairs - apply the terrain_name material
      terrain_mesh->begin(terrain_material_name);

      for (size_t i = 0; i + step <= page_size; i += step) {
        size_t is = i + step;
        for (size_t j = 0; j + step <= page_size; j += step) {
          size_t js = j + step;

          real_pair angle[4];
          Real height[4];
          size_t x[4];
          size_t y[4];
          size_t i_coords;

          // from bottom left anticlockwise
          x[0] = (origin_x + i);
          y[0] = (origin_y + j);
          x[1] = (origin_x + i);
          y[1] = (origin_y + js);
          x[2] = (origin_x + is);
          y[2] = (origin_y + js);
          x[3] = (origin_x + is);
          y[3] = (origin_y + j);

          for (i_coords = 0; i_coords < 4; ++i_coords) {
            angle[i_coords] = terrain->getAngle(x[i_coords] % texture_size_w,
                                                y[i_coords] % texture_size_h);
            height[i_coords] = terrain->getHeight(x[i_coords] % texture_size_w,
                                                  y[i_coords] % texture_size_h);
          }

          // first bottom-left vertex
          i_coords = 0;
          terrain_mesh->position(i * metres_per_pixel, height[i_coords],
                                 j * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(texture_size_w),
                                     y[i_coords] / Real(texture_size_h));
          terrain_mesh->textureCoord(i / detail_density, j / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // first bottom-right vertex
          i_coords = 1;
          terrain_mesh->position(i * metres_per_pixel, height[i_coords],
                                 js * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(texture_size_w),
                                     y[i_coords] / Real(texture_size_h));
          terrain_mesh->textureCoord(i / detail_density, js / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // first top-right vertex
          i_coords = 2;
          terrain_mesh->position(is * metres_per_pixel, height[i_coords],
                                 js * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(texture_size_w),
                                     y[i_coords] / Real(texture_size_h));
          terrain_mesh->textureCoord(is / detail_density, js / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // second top-left vertex
          i_coords = 3;
          terrain_mesh->position(is * metres_per_pixel, height[i_coords],
                                 j * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(texture_size_w),
                                     y[i_coords] / Real(texture_size_h));
          terrain_mesh->textureCoord(is / detail_density, j / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // second bottom-left vertex
          i_coords = 0;
          terrain_mesh->position(i * metres_per_pixel, height[i_coords],
                                 j * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(texture_size_w),
                                     y[i_coords] / Real(texture_size_h));
          terrain_mesh->textureCoord(i / detail_density, j / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // second top-right vertex
          i_coords = 2;
          terrain_mesh->position(is * metres_per_pixel, height[i_coords],
                                 js * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(texture_size_w),
                                     y[i_coords] / Real(texture_size_h));
          terrain_mesh->textureCoord(is / detail_density, js / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);
        }
      }

      // finished creating traignles
      terrain_mesh->end();

      // terrain entity
      terrain_mesh->setQueryFlags(query_mask_ignore);
      terrain_mesh->setCastShadows(false);

      // attach to a scene node
      Ogre::SceneNode* cell_node = terrain_node->createChildSceneNode();
      cell_node->attachObject(terrain_mesh);

      // position cells in agrid
      cell_node->setPosition(origin_x * metres_per_pixel, 0,
                             origin_y * metres_per_pixel);
    }
  }
}

/** @brief fills the vector with indexes of cells within the given radius from a cell
 * you could cut off the corner indexes for a big radius but it's hardly worth the bother
 * at radius 4 you could save one cell, OK, TODO: cut them off
 */
void GameArena::getCellIndexesWithinRadius(const size_t_pair    a_index,
                                           vector<size_t_pair>& indexes,
                                           const Real           a_radius)
{
  // round up and add half the cell size to get possible hits from without the radius
  int cell_radius = (a_radius + size_of_arena_cell * 0.5) / size_of_arena_cell + 1;

  // push the centre cell
  indexes.push_back(a_index);

  // centre for the spiral (measure from the edge, so even radius 0 gets a cell)
  const size_t& c_i = a_index.first;
  const size_t& c_j = a_index.second;

  // this goes in a spiral way putting in indexes closer to first and further away last
  // every time it checks if the index is valid
  for (size_t r = 1; r <= cell_radius; ++r) {
    if (c_i + r >= 0 && c_i + r < num_of_arena_cells_w) {
      for (size_t i = -r; i <= r; ++i) {
        if (c_j + i >= 0 && c_j + i < num_of_arena_cells_h) {
          indexes.push_back(make_pair(c_i + r, c_j + i));
        }
      }
    }
    if (c_i - r >= 0 && c_i - r < num_of_arena_cells_w) {
      for (size_t i = -r; i <= r; ++i) {
        if (c_j + i >= 0 && c_j + i < num_of_arena_cells_h) {
          indexes.push_back(make_pair(c_i - r, c_j + i));
        }
      }
    }
    if (c_j + r >= 0 && c_j + r < num_of_arena_cells_h) {
      for (size_t i = -(r - 1); i <= (r - 1); ++i) {
        if (c_i + i >= 0 && c_i + i < num_of_arena_cells_w) {
          indexes.push_back(make_pair(c_i + i, c_j + r));
        }
      }
    }
    if (c_j - r >= 0 && c_j - r < num_of_arena_cells_h) {
      for (size_t i = -(r - 1); i <= (r - 1); ++i) {
        if (c_i + i >= 0 && c_i + i < num_of_arena_cells_w) {
          indexes.push_back(make_pair(c_i + i, c_j - r));
        }
      }
    }
  }
}

/** @brief moves the object's pointer to the new cell if needed
 */
bool GameArena::updateCellIndex(Corpus* a_thing)
{
  bool cell_changed = false;
  // if it's not within the limits of the arena rectiify the position
  if (isOutOfBounds(a_thing->XYZ)) {
    a_thing->goOutOfBounds();
  }
  // get the cell index based on position
  size_t_pair new_cell_index = getCellIndex(a_thing->XYZ.x, a_thing->XYZ.z);
  const size_t_pair cell_index = a_thing->CellIndex;

  if (cell_index != new_cell_index) {
    // remove the old cell
    corpus_cells[cell_index.first][cell_index.second].remove(a_thing);

    // if no objects left remove from the list of live cells
    if (corpus_cells[cell_index.first][cell_index.second].size() == 0) {
      LiveCorpusCells.remove(&corpus_cells[cell_index.first][cell_index.second]);
    }

    // set the new index
    a_thing->CellIndex = new_cell_index;

    // add to the new cell
    corpus_cells[cell_index.first][cell_index.second].push_back(a_thing);

    // if it's the first object inset the cell into the list of live cells
    if (corpus_cells[cell_index.first][cell_index.second].size() == 1) {
      LiveCorpusCells.push_back(&corpus_cells[cell_index.first][cell_index.second]);
      cell_changed = true;
    }
  }

  return cell_changed;
}
