// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game.h"
#include "game_hud.h"
#include "files_handler.h"
#include "game_timer.h"
#include "text_store.h"
#include "game_camera.h"
#include "input_handler.h"
#include "game_controller.h"
#include "game_arena.h"
#include "corpus_manager.h"
#include "faction_manager.h"
#include "formation_manager.h"
#include "ai_manager.h"
#include "building_factory.h"
#include "unit_factory.h"
#include "projectile_factory.h"
#include "particle_manager.h"
#include "collision_handler.h"
#include "status_computer.h"

Game* gGame = NULL;

// 64fps limit for the game
#define FPS_INTERVAL (16)

const string VERSION_NUMBER = FilesHandler::getStringFromFile(DATA_DIR + "version");

Ogre::Root*             Game::OgreRoot = NULL;
Ogre::Viewport*         Game::OgreViewport = NULL;
Ogre::RenderWindow*     Game::OgreWindow = NULL;
Ogre::SceneManager*     Game::Scene = NULL;
InputHandler*           Game::Input = NULL;
TextStore*              Game::Text = NULL;
GameCamera*             Game::Camera = NULL;
CorpusManager*          Game::Corpus = NULL;
FormationManager*       Game::Formation = NULL;
FactionManager*         Game::Faction = NULL;
GameArena*              Game::Arena = NULL;
AIManager*              Game::AI = NULL;
ParticleManager*        Game::Particle = NULL;
UnitFactory*            Game::Unit = NULL;
BuildingFactory*        Game::Building = NULL;
ProjectileFactory*      Game::Projectile = NULL;
CollisionHandler*       Game::Collision = NULL;
GameTimer*              Game::Timer = NULL;
GameHud*                Game::Hud = NULL;
vector<GameController*> Game::Controllers;

game_state Game::State = game_state::game_state_init;
ulint Game::NewTime = 0;
ulint Game::LastTime = 0;
ulint Game::RealTime = 0;
Real Game::Delta = 0;
Real Game::Fps = 0;
size_t Game::UniqueId = 0;
bool Game::DebugMode = false;

Game::~Game()
{
}

Game::Game()
{
  Game::Text = new TextStore();
}

const string& Game::getVersion()
{
  return VERSION_NUMBER;
}

/** accessor for files not defining extern gGame
 */
Game* Game::instance()
{
  return gGame;
}

int Game::init(bool a_debug_on)
{
  gGame = new Game();

  DebugMode = a_debug_on;

  return 0;
}

void Game::log(const string& a_text)
{
  cout << a_text << endl;
}

/** @brief starts the game loop
 * @todo: launch menu system instead of the game from the start
 */
void Game::run()
{
  // OGRE initialisation
  OgreRoot = new Ogre::Root("", DATA_DIR + CONFIG_DIR + "graphics", "ogre.log");

  // load the OpenGL render system
#ifdef PLATFORM_WIN32
#ifdef PLATFORM_DEBUG
  OgreRoot->loadPlugin("RenderSystem_GL_d.dll");
  OgreRoot->loadPlugin("Plugin_ParticleFX_d.dll");
#else
  OgreRoot->loadPlugin("RenderSystem_GL.dll");
  OgreRoot->loadPlugin("Plugin_ParticleFX.dll");
#endif
#else
  OgreRoot->loadPlugin("./OGRE/RenderSystem_GL");
  OgreRoot->loadPlugin("./OGRE/Plugin_ParticleFX");
#endif

  Ogre::OverlaySystem* overlay_system = new Ogre::OverlaySystem();

  if (OgreRoot->restoreConfig() || OgreRoot->showConfigDialog()) {
    OgreWindow = OgreRoot->initialise(true, string("Metal Crusade ") + VERSION_NUMBER, "");

    Game::Scene = OgreRoot->createSceneManager(Ogre::ST_GENERIC);
    // setup shadow type - needs to be done first
    Game::Scene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
    Game::Scene->addRenderQueueListener(overlay_system);

    // set resources
    Ogre::ResourceGroupManager& res_mngr = Ogre::ResourceGroupManager::getSingleton();
    res_mngr.addResourceLocation(DATA_DIR + "model", "FileSystem", "models");
    res_mngr.addResourceLocation(DATA_DIR + "texture", "FileSystem", "textures");
    res_mngr.addResourceLocation(DATA_DIR + "texture/particle", "FileSystem", "particles");
    res_mngr.addResourceLocation(DATA_DIR + "texture/font", "FileSystem", "textures");
    res_mngr.addResourceLocation(DATA_DIR + "terrain", "FileSystem", "terrain");
    res_mngr.addResourceLocation(DATA_DIR + "model/interface", "FileSystem", "models");
    // set units TODO: needs to be automatic
    res_mngr.addResourceLocation(DATA_DIR + "model/husar", "FileSystem", "models");
    res_mngr.addResourceLocation(DATA_DIR + "model/building_test_01", "FileSystem", "models");
    res_mngr.addResourceLocation(DATA_DIR + "model/building_test_02", "FileSystem", "models");

    // load resources
    res_mngr.initialiseResourceGroup("models");
    res_mngr.initialiseResourceGroup("textures");
    res_mngr.initialiseResourceGroup("particles");
    res_mngr.initialiseResourceGroup("terrain");

    Game::Camera = new GameCamera(); // needs scene manager to exist

    // init viewport with the camera
    OgreViewport = OgreWindow->addViewport(Camera->OgreCamera);
    OgreViewport->setBackgroundColour(Ogre::ColourValue::Black);

    // needs to be called after camera, window, viewport, scene manager
    Game::Input = new InputHandler();

    // bind input to first controller
    Controllers.push_back(new GameController("Player1"));
    Game::Input->bindController(Controllers[0]);

    // game time init
    Game::Timer = new GameTimer(OgreRoot->getTimer());
    LastTime = Game::Timer->getTicks();
    RealTime = 0;

    // create the world and objects
    Game::Formation = new FormationManager();
    Game::Faction = new FactionManager();

    enterArena();

    // load map and start main loop
    if (Arena->loadArena("test_arena") == 0) {
      OgreRoot->addFrameListener(this);  // gets frameRenderingQueued called every frame
      State = game_state_playing;
      Game::Hud->activate(true);
      Game::Input->resize();
      OgreRoot->startRendering(); // hands over main loop to ogre
    }
  }
}

void Game::enterArena()
{
  Game::Arena = new GameArena();

  Game::Corpus = new CorpusManager();
  Game::AI = new AIManager();
  Game::Building = new BuildingFactory();
  Game::Projectile = new ProjectileFactory();
  Game::Unit = new UnitFactory();
  Game::Particle = new ParticleManager();
  Game::Collision = new CollisionHandler();
  Game::Hud = new GameHud();
}

void Game::exitArena()
{
  delete Game::AI;
  delete Game::Corpus;
  delete Game::Building;
  delete Game::Projectile;
  delete Game::Unit;
  delete Game::Particle;
  delete Game::Collision;
  delete Game::Hud;

  delete Arena;
  Game::Arena = NULL;
}

/** @brief captures input from devices
 * @todo: add support for more devices and later network goes here as well
 */
void Game::input()
{
  Game::Input->capture();
}

/** @brief main loop is handled by OGRE and this gets called once every frame
 * after the data is ready to be sent to to the GPU
 */
bool Game::frameRenderingQueued(const Ogre::FrameEvent& /*aEvt*/)
{
  fpsCalc();

  input();

  if (State == game_state_playing) {
    // use internal clock
    NewTime = Timer->getTicks();
    ulint d_ticks = NewTime - LastTime;
    LastTime = NewTime;

    // main game loop
    logic(d_ticks);

  } else if (State == game_state_pause) {

  } else if (State == game_state_closing) {
    return false;

  } else {
    return false;
  }

  // pad out time for steady FPS
  //limitFPS();

  // renders frame at return
  return true;
}

/** @brief physics and the rest of the simulation
 */
void Game::logic(ulint a_ticks)
{
  if (a_ticks > 0) {  // don't update if paused
    // ticks to ticks per second
    Delta = Real(a_ticks) / Real(1000);

    // update game objects
    Game::Arena->update(Delta);
    Game::Faction->update(Delta);
    Game::Formation->update(Delta);
    Game::Particle->update(Delta);
    Game::Camera->update(Delta);
    Game::Corpus->update(Delta);

    // show the overlays (2D hud)
    Game::Hud->update(Delta);

    Game::AI->update(Delta);
  }
}

/** @brief limits the game (to 64 fps atm)
 */
void Game::limitFPS()
{
  // get actual ticks
  ulint new_real_time = OgreRoot->getTimer()->getMilliseconds();

  // limit the fps to 64 ticks
#ifdef PLATFORM_WIN32
  if (new_real_time - RealTime < FPS_INTERVAL) {
    DWORD sleep_time = (FPS_INTERVAL - (new_real_time - RealTime));
    Sleep(sleep_time);
  }
#else
  if (new_real_time - RealTime < FPS_INTERVAL) {
    timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = (FPS_INTERVAL - (new_real_time - RealTime)) * 1000;
    nanosleep(&sleep_time, &sleep_time);
  }
#endif
}

/** @brief calculates fps
 */
void Game::fpsCalc()
{
  // get actual ticks
  ulint new_real_time = OgreRoot->getTimer()->getMilliseconds();

  // smooth displayed fps over time
  Fps = 0.8 * Fps + 0.2 * (Real(1000) / Real(new_real_time - RealTime));
  RealTime = new_real_time;

  // temp, show the fps
  Game::Hud->status->setLine(string("$eFPS: ") + intoString(Fps), 0, 20, 50);
}

/** @brief terminal error - die
 */
void Game::kill(string aGoodbye)
{
  cout << aGoodbye << endl;
  exit(1);
}

void Game::setGameState(game_state a_state)
{
  if (a_state == game_state_pause) {
    Game::Hud->pause();
  }
  State = a_state;
}
