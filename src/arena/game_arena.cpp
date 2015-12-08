// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game_arena.h"
#include "game.h"
#include "game_hud.h"
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
 #include "game_hud.h"*/

// cell size
const int real_page_size = 256; // in metres
const usint page_size = real_page_size / metres_per_pixel; // in pixels
// white colour means 16 * scale metres high
const Real max_height = 16 * metres_per_pixel; // 128

const string terrain_material_name = "terrain_material";

GameArena::GameArena()
{
  // create lights
  Sunlight = Game::Scene->createLight("sunlight");
  Backlight = Game::Scene->createLight("backlight");
  GroundLight = Game::Scene->createLight("ground_light");

  // no ambient light - we use ground light to fake that
  Game::Scene->setAmbientLight(Ogre::ColourValue::Black);
  // this could possibly be moved to update light
  Game::Scene->setShadowColour(Ogre::ColourValue(0.3, 0.3, 0.4));

  // sunlight casting shadows
  Sunlight->setType(Ogre::Light::LT_DIRECTIONAL);
  Sunlight->setPosition(Vector3(100, 200, -100)); // TODO: change to spot for texture shadows
  Sunlight->setCastShadows(true);
  Backlight->setType(Ogre::Light::LT_DIRECTIONAL);
  Backlight->setCastShadows(false);
  GroundLight->setType(Ogre::Light::LT_DIRECTIONAL);
  GroundLight->setCastShadows(false);
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
  destroyModel(TerrainNode);

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
  Sunlight->setDiffuseColour(r, g, b);
  Sunlight->setDirection(-0.5, -1, 0.5);

  // backlight
  Backlight->setDiffuseColour(b * 0.25, g * 0.25, r * 0.5);
  Backlight->setDirection(1, -0.5, -1);

  // fake ambient TODO: take dominant ground colour
  GroundLight->setDiffuseColour(0.8, 0.5, 0.4);
  GroundLight->setDirection(0.1, 1, 0.3);
}

/** @brief returns the height of the arena at this point
 */
Real GameArena::getHeight(Real a_x,
                          Real a_y)
{
  return TerrainData->getHeight(a_x, a_y);
}

/** @brief loads a map when you enter a location and populates it with stuff
 * @todo: loadArena from a script file and move this to a class
 */
int GameArena::loadArena(const string& arena_name)
{
  // TODO: read from file obviously
  Gravity = 10; // m/s^2

  // create the terrain
  TerrainData = TerrainGenerator::generateTerrain("test");

  // todo: this might go the other way around, so that the terrain adapts to the arena size
  // size of the map in metres
  SceneSizeW = TerrainData->SizeW * metres_per_pixel;
  SceneSizeH = TerrainData->SizeH * metres_per_pixel;

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
/*
   // temp buildings
   Game::Building->spawnSceneryBuidling(Vector3(240, 0, 220), "building_test_02");

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
 */

  // fake game startup from code - ought to be read from file
  Crusader* player_unit = Game::Unit->spawnCrusader(Vector3(220, 0, 280), "base_husar_cavalry");

  player_unit->assignController(Game::getGameController(0));

  // set player unit controller to local input
  player_formation->joinFormation(player_unit);

  // create the hud according to the unit you're in - HUD NEEDS THE CONTROLLER to be assigned!
  Game::Hud->loadHud(static_cast<Unit*>(player_unit));
/*
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
 */
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
  NumArenaCellsW = SceneSizeW / size_of_arena_cell;
  NumArenaCellsH = SceneSizeH / size_of_arena_cell;

  // set size for the cells holding the object on the map
  CorpusCells = vector<vector<list<Corpus*> > >(NumArenaCellsW,
                                                vector<list<Corpus*> >(NumArenaCellsH));
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

  const size_t TextureSizeW = TerrainData->SizeW;
  const size_t TextureSizeH = TerrainData->SizeH;

  // calculate number of terrain pages
  const usint num_of_pages_w = TextureSizeW / page_size;
  const usint num_of_pages_h = TextureSizeH / page_size;

  // node to hold all terrain cells
  TerrainNode = Game::Scene->getRootSceneNode()->createChildSceneNode();

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
  //pass->createTextureUnitState()->setTextureName(terrain_name+"_colour.tga");
  //pass->getTextureUnitState(1)->setTextureCoordSet(0);
  // modulate colour and detail
  // pass->getTextureUnitState(1)->setColourOperation(Ogre::LBO_MODULATE);

  // create cells containing portions of the terrain so it can be culled when not on screen
  for (size_t k = 0; k < num_of_pages_w; ++k) {
    for (size_t l = 0; l < num_of_pages_h; ++l) {
      // offsets for this cell
      size_t origin_x = k * page_size;
      size_t origin_y = l * page_size;

      // create the mesh part
      Ogre::ManualObject* terrain_mesh = Game::Scene->createManualObject(
        "terrain_mesh_" + intoString(l)
        + "_" + intoString(k));

      // create the mesh manually by defining triangle pairs - apply the terrain_name material
      terrain_mesh->begin(terrain_material_name); // arg is material name

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
            angle[i_coords] = TerrainData->getAngle(x[i_coords] % TextureSizeW,
                                                    y[i_coords] % TextureSizeH);
            height[i_coords] = TerrainData->getHeight(x[i_coords] % TextureSizeW,
                                                      y[i_coords] % TextureSizeH);
          }

          // first bottom-left vertex
          i_coords = 0;
          terrain_mesh->position(i * metres_per_pixel, height[i_coords],
                                 j * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(TextureSizeW),
                                     y[i_coords] / Real(TextureSizeH));
          terrain_mesh->textureCoord(i / detail_density, j / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // first bottom-right vertex
          i_coords = 1;
          terrain_mesh->position(i * metres_per_pixel, height[i_coords],
                                 js * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(TextureSizeW),
                                     y[i_coords] / Real(TextureSizeH));
          terrain_mesh->textureCoord(i / detail_density, js / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // first top-right vertex
          i_coords = 2;
          terrain_mesh->position(is * metres_per_pixel, height[i_coords],
                                 js * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(TextureSizeW),
                                     y[i_coords] / Real(TextureSizeH));
          terrain_mesh->textureCoord(is / detail_density, js / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // second top-left vertex
          i_coords = 3;
          terrain_mesh->position(is * metres_per_pixel, height[i_coords],
                                 j * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(TextureSizeW),
                                     y[i_coords] / Real(TextureSizeH));
          terrain_mesh->textureCoord(is / detail_density, j / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // second bottom-left vertex
          i_coords = 0;
          terrain_mesh->position(i * metres_per_pixel, height[i_coords],
                                 j * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(TextureSizeW),
                                     y[i_coords] / Real(TextureSizeH));
          terrain_mesh->textureCoord(i / detail_density, j / detail_density);
          terrain_mesh->normal(angle[i_coords].first, angle[i_coords].second, 0.5);

          // second top-right vertex
          i_coords = 2;
          terrain_mesh->position(is * metres_per_pixel, height[i_coords],
                                 js * metres_per_pixel);
          terrain_mesh->textureCoord(x[i_coords] / Real(TextureSizeW),
                                     y[i_coords] / Real(TextureSizeH));
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
      Ogre::SceneNode* cell_node = TerrainNode->createChildSceneNode();
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
void GameArena::getCellIndexesWithinRadius(const Vector3&       a_xyz,
                                           vector<size_t_pair>& indexes,
                                           const Real           a_radius)
{
  size_t_pair index = getCellIndex(a_xyz);
  // round up and add half the cell size to get possible hits from without the radius
  int cell_radius = (a_radius + size_of_arena_cell * 0.5) / size_of_arena_cell + 1;

  // push the centre cell
  indexes.push_back(index);

  // centre for the spiral (measure from the edge, so even radius 0 gets a cell)
  const size_t& c_i = index.first;
  const size_t& c_j = index.second;

  // this goes in a spiral way putting in indexes closer to first and further away last
  // every time it checks if the index is valid
  for (size_t r = 1; r <= cell_radius; ++r) {
    if (c_i + r >= 0 && c_i + r < NumArenaCellsW) {
      for (size_t i = -r; i <= r; ++i) {
        if (c_j + i >= 0 && c_j + i < NumArenaCellsH) {
          indexes.push_back(make_pair(c_i + r, c_j + i));
        }
      }
    }
    if (c_i - r >= 0 && c_i - r < NumArenaCellsW) {
      for (size_t i = -r; i <= r; ++i) {
        if (c_j + i >= 0 && c_j + i < NumArenaCellsH) {
          indexes.push_back(make_pair(c_i - r, c_j + i));
        }
      }
    }
    if (c_j + r >= 0 && c_j + r < NumArenaCellsH) {
      for (size_t i = -(r - 1); i <= (r - 1); ++i) {
        if (c_i + i >= 0 && c_i + i < NumArenaCellsW) {
          indexes.push_back(make_pair(c_i + i, c_j + r));
        }
      }
    }
    if (c_j - r >= 0 && c_j - r < NumArenaCellsH) {
      for (size_t i = -(r - 1); i <= (r - 1); ++i) {
        if (c_i + i >= 0 && c_i + i < NumArenaCellsW) {
          indexes.push_back(make_pair(c_i + i, c_j - r));
        }
      }
    }
  }
}

void GameArena::deregisterObject(Corpus* a_thing)
{
  a_thing->OwnerEntity = NULL;
  if (a_thing->OnArena) {
    a_thing->OnArena = false;
    removeCorpusFromCell(a_thing);
  }
}

void GameArena::registerObject(Corpus* a_thing)
{
  if (!a_thing->OnArena) {
    a_thing->OnArena = true;
    a_thing->CellIndex = getCellIndex(a_thing->XYZ);
    insertCorpusIntoCell(a_thing);
  }
}

void GameArena::removeCorpusFromCell(Corpus* a_thing)
{
  const size_t_pair& cell_index = a_thing->CellIndex;

  // remove the old cell
  CorpusCells[cell_index.first][cell_index.second].remove(a_thing);

  // if no objects left remove from the list of live cells
  if (CorpusCells[cell_index.first][cell_index.second].size() == 0) {
    LiveCorpusCells.remove(&CorpusCells[cell_index.first][cell_index.second]);
  }
}

void GameArena::insertCorpusIntoCell(Corpus* a_thing)
{
  const size_t_pair& cell_index = a_thing->CellIndex;
  // add to the new cell
  CorpusCells[cell_index.first][cell_index.second].push_back(a_thing);

  // if it's the first object inset the cell into the list of live cells
  if (CorpusCells[cell_index.first][cell_index.second].size() == 1) {
    LiveCorpusCells.push_back(&CorpusCells[cell_index.first][cell_index.second]);
  }
}

/** @brief moves the object's pointer to the new cell if needed
 */
bool GameArena::updateCellIndex(Corpus* a_thing)
{
  bool cell_changed = false;

  if (isOutOfBounds(a_thing->XYZ)) {
    // if it's not within the limits of the arena the position has just been fixed
    // so it's safe to continue to get indexes
    a_thing->OutOfBounds = true;
  }

  // get the cell index based on position
  const size_t_pair new_cell_index = getCellIndex(a_thing->XYZ);

  if (a_thing->CellIndex != new_cell_index) {
    cell_changed = true;
    removeCorpusFromCell(a_thing);
    // set the new index
    a_thing->CellIndex = new_cell_index;
    insertCorpusIntoCell(a_thing);
  }

  return cell_changed;
}
