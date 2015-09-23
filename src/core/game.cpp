// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game.h"
#include "files_handler.h"
#include "timer.h"
#include "text_store.h"
#include "game_camera.h"
#include "input_handler.h"
#include "game_controller.h"
#include "game_arena.h"
#include "faction_manager.h"
#include "formation_manager.h"

Game* gGame = 0;

// 64fps limit for the game
#define FPS_INTERVAL (16)

const string VERSION_NUMBER = FS::getStringFromFile(DATA_DIR + "version");

Ogre::Root* Game::OgreRoot = NULL;
Ogre::Viewport* Game::OgreViewport = NULL;
Ogre::RenderWindow* Game::OgreWindow = NULL;
Ogre::SceneManager* Game::OgreScene = NULL;

InputHandler* Game::Input = NULL;
TextStore* Game::Text = NULL;
GameCamera* Game::Camera = NULL;
FormationManager* Game::Formation = NULL;
FactionManager* Game::Faction = NULL;
GameArena* Game::Arena = NULL;
AIManager* Game::AI = NULL;
ParticleManager* Game::Particle = NULL;
UnitFactory* Game::Unit = NULL;
BuildingFactory* Game::Building = NULL;
ProjectileFactory* Game::Projectile = NULL;

vector<GameController*> Game::Controllers;
game_state Game::State;
Timer* Game::GameTimer = NULL;
ulint Game::NewTime;
ulint Game::LastTime;
ulint Game::RealTime;
Ogre::Real Game::Fps;
size_t Game::UniqueId;
bool Game::DebugMode;

Game::Game()
{
  FS::init();

  Text = new TextStore();

  Fps = 0;
  UniqueId = 0;
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

int Game::init(bool aDebugOn)
{
  gGame = new Game();

  DebugMode = aDebugOn;

  return 0;
}

void Game::log(const string& aText)
{
  cout << aText << endl;
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

  if (OgreRoot->restoreConfig() || OgreRoot->showConfigDialog()) {
    OgreWindow = OgreRoot->initialise(true, string("Metal Crusade ") + VERSION_NUMBER, "");

    OgreScene = OgreRoot->createSceneManager(Ogre::ST_GENERIC);
    // setup shadow type - needs to be done first
    OgreScene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);

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

    Camera = new GameCamera(); // needs scene manager to exist

    // init viewport with the camera
    OgreViewport = OgreWindow->addViewport(Camera->getOgreCamera());
    OgreViewport->setBackgroundColour(Ogre::ColourValue::Black);

    // needs to be called after camera, window, viewport, scene manager
    Input = new InputHandler();

    // bind input to first controller
    Controllers.push_back(new GameController("Player1"));
    Input->bindController(Controllers[0]);

    // game time init
    GameTimer = new Timer(OgreRoot->getTimer());
    LastTime = GameTimer->getTicks();
    RealTime = 0;

    // create the world and objects
    Formation = new FormationManager();
    Faction = new FactionManager();

    enterArena();

    // load map and start main loop
    if (Arena->loadArena("test_arena") == 0) {
      OgreRoot->addFrameListener(this);  // gets frameRenderingQueued called every frame
      State = game_state_pause;
      // Game::hud->activate(true);
      OgreRoot->startRendering(); // hands over main loop to ogre
    }

  }
}

void Game::enterArena()
{
  Arena = new GameArena();

  Game::AiManager = new AiManager();
  Game::Building = new BuildingFactory();
  Game::Projectile = new ProjectileFactory();
  Game::Unit = new UnitFactory();
  Game::Particle = new ParticleManager();
  //Game::hud = new Hud();
  Game::collider = new CollisionHandler();
}

void Game::exitArena()
{
  delete Game::AiManager;
  delete Game::Building;
  delete Game::Projectile;
  delete Game::Unit;
  delete Game::Particle;
  //delete Game::Hud;
  delete Game::Collider;

  delete Arena;
  Arena = NULL;
}

/** @brief captures input from devices
  * @todo: add support for more devices and later network goes here as well
  */
void Game::input()
{
  // input_handler->capture();
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
    NewTime = GameTimer->getTicks();
    int d_ticks = NewTime - LastTime;
    LastTime = NewTime;

    // main game loop
    logic(d_ticks);

  } else if (State == game_state_pause) {
    GameTimer->pause();
    // hud->pause();

  } else if (State == game_state_closing) {
    return false;

  } else {
    return false;
  }

  // pad out time for steady FPS
  limitFPS();

  // renders frame at return
  return true;
}

/** @brief physics and the rest of the simulation
  */
void Game::logic(int aDTicks)
{
  if (aDTicks > 0) {  // don't update if paused
    // ticks to ticks per second
    Ogre::Real dt = Ogre::Real(aDTicks) / Ogre::Real(1000);

    // update game objects
    // unit_factory->update(dt);
    // projectile_factory->update(dt);
    // particle_factory->update(dt);
    // faction_factory->update(dt);
    // formation_factory->update(dt);

    // move camera
    // camera->update(dt);

    // show the overlays (2D hud)
    // hud->update(dt);

    // find and resolve collisions
    // collider->update(dt);

    // ai_factory->update();
    // ai called as the last thing in the frame
    // so that I can later use it to pad the execution time with ai
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
  Fps = 0.8 * Fps + 0.2 * (Ogre::Real(1000) / Ogre::Real(new_real_time - RealTime));
  RealTime = new_real_time;

  // temp, show the fps
  // hud->status->setLine(string("$eFPS: ") + intoString(Fps), 0, 20, 50);
}

/** @brief terminal error - die
  */
void Game::kill(string aGoodbye)
{
  cout << aGoodbye << endl;
  exit(1);
}
