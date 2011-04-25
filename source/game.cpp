//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "game.h"
#include "arena.h"
#include "query_mask.h"
#include "files_handler.h"
#include "collision_handler.h"
#include "input_handler.h"
#include "unit_factory.h"
#include "ai_factory.h"
#include "corpus_factory.h"
#include "particle_factory.h"
#include "projectile_factory.h"
#include "formation_factory.h"
#include "timer.h"
#include "text_store.h"
#include "hud.h"
#include "game_controller.h"
#include "camera.h"
#include "status_computer.h"

//define static members
CorpusFactory* Game::corpus_factory;
ParticleFactory* Game::particle_factory;
ProjectileFactory* Game::projectile_factory;
UnitFactory* Game::unit_factory;
FormationFactory* Game::formation_factory;
AIFactory* Game::ai_factory;
Ogre::Root* Game::ogre;
Ogre::Viewport* Game::viewport;
Ogre::RenderWindow* Game::render_window;
Ogre::SceneManager* Game::scene;
Camera* Game::camera;
Arena* Game::arena;
Hud* Game::hud;
InputHandler* Game::input_handler;
TextStore* Game::text;
CollisionHandler* Game::collider;

// 64fps limit for the game
const unsigned int fps_interval = 16;

//version number
const string version_number = FilesHandler::getStringFromFile(data_dir+"version");

/** singleton
  */
Game* Game::instance()
{
    return inst;
}

Game* Game::inst = 0;

int Game::instantiate()
{
    inst = new Game();
    return 0;
}

/** @brief starts the game loop
  * @todo: launch menu system instead of the game from the start
  */
void Game::init()
{
    //singletons
    FilesHandler::instantiate();

    //OGRE initialisation
    ogre = new Ogre::Root("", data_dir+config_dir+"graphics", "ogre.log");

    //load the OpenGL render system
    #ifdef PLATFORM_WIN32
        #ifdef PLATFORM_DEBUG
            ogre->loadPlugin("RenderSystem_GL_d.dll");
            ogre->loadPlugin("Plugin_ParticleFX_d.dll");
        #else
            ogre->loadPlugin("RenderSystem_GL.dll");
            ogre->loadPlugin("Plugin_ParticleFX.dll");
        #endif
    #else
        ogre->loadPlugin("/usr/local/lib/OGRE/RenderSystem_GL");
        ogre->loadPlugin("/usr/local/lib/OGRE/Plugin_ParticleFX");
    #endif

    if(ogre->restoreConfig() || ogre->showConfigDialog()) {
        //DO NOT change order of creation! you might upset the zen of the feng-shui layout
        //create window
        render_window = ogre->initialise(true, string("Metal Crusade")+" "+version_number, "");

        //create scene manager
        scene = ogre->createSceneManager(Ogre::ST_GENERIC);

        //setup shadow type - needs to be done first
        scene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);

        //set resources
        Ogre::ResourceGroupManager& resource_mngr = Ogre::ResourceGroupManager::getSingleton();
        resource_mngr.addResourceLocation(data_dir+"model", "FileSystem", "models");
        resource_mngr.addResourceLocation(data_dir+"texture", "FileSystem", "textures");
        resource_mngr.addResourceLocation(data_dir+"texture/particle", "FileSystem", "particles");
        resource_mngr.addResourceLocation(data_dir+"texture/font", "FileSystem", "textures");
        resource_mngr.addResourceLocation(data_dir+"terrain", "FileSystem", "terrain");
        resource_mngr.addResourceLocation(data_dir+"model/interface", "FileSystem", "models");
        //set units TODO: needs to be automatic
        resource_mngr.addResourceLocation(data_dir+"model/husar", "FileSystem", "models");
        resource_mngr.addResourceLocation(data_dir+"model/building_test_01", "FileSystem",
                                          "models");
        resource_mngr.addResourceLocation(data_dir+"model/building_test_02", "FileSystem",
                                          "models");

        //load resources
        resource_mngr.initialiseResourceGroup("models");
        resource_mngr.initialiseResourceGroup("textures");
        resource_mngr.initialiseResourceGroup("particles");
        resource_mngr.initialiseResourceGroup("terrain");

        //create the text store
        text = new TextStore();

        //setup the camera
        camera = new Camera(); //needs scene manager to exist

        //init viewport with the camera
        viewport = render_window->addViewport(camera->getOgreCamera());
        viewport->setBackgroundColour(Ogre::ColourValue::Black);

        //needs to be called after camera, window, viewport, scene manager
        input_handler = new InputHandler();

        //bind input to first controller
        game_controllers[0] = new GameController("Player1");
        input_handler->bindController(game_controllers[0]);

        //game time init
        timer = new Timer(ogre->getTimer());
        last_time = timer->getTicks();
        real_time = 0;

        //create the world and objects
        formation_factory = new FormationFactory();
        arena = new Arena();

        //load map and start main loop
        if (arena->loadArena("test_arena") == 0) {
            ogre->addFrameListener(this); //gets frameRenderingQueued called every frame
            state = game_state_pause;
            Game::hud->activate(true);
            ogre->startRendering(); //hands over main loop to ogre
        }
    }
}

/** @brief captures input from devices
  * @todo: add support for more devices and later network goes here as well
  */
void Game::input()
{
    input_handler->capture();
}

/** @brief main loop is handled by OGRE and this gets called once every frame
  * after the data is ready to be sent to to the GPU
  */
bool Game::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    fpsCalc();

    input();

    if (state == game_state_playing) {
        //use internal clock
        new_time = timer->getTicks();
        int d_ticks = new_time - last_time;
        last_time = new_time;

        //main game loop
        logic(d_ticks);

    } else if (state == game_state_pause) {
        timer->pause();
        hud->pause();

    } else if (state == game_state_closing) {
        return false;

    } else {
        return false;
    }

    //padd out time for steady FPS
    limitFPS();

    //renders frame at return
    return true;
}

/** @brief physics and the rest of the simulation
  */
void Game::logic(int a_d_ticks)
{
    if (a_d_ticks > 0) { //don't update if paused
        //ticks to ticks per second
        Ogre::Real dt = Ogre::Real(a_d_ticks) / Ogre::Real(1000);

        //update game objects
        unit_factory->update(dt);
        projectile_factory->update(dt);
        particle_factory->update(dt);

        //move camera
        camera->update(dt);

        //show the overlays (2D hud)
        hud->update(dt);

        //find and resolve collisions
        collider->update(dt);

        ai_factory->update();
        //ai called as the last thing in the frame
        //so that I can later use it to pad the execution time with ai
    }
}

/** @brief limits the game (to 64 fps atm)
  */
void Game::limitFPS()
{
    //get actual ticks
    ulint new_real_time = ogre->getTimer()->getMilliseconds();

    #ifdef PLATFORM_WIN32
        //limit the fps to 64 ticks for stable step
        if (new_real_time - real_time < fps_interval) {
            DWORD sleep_time = (fps_interval - (new_real_time - real_time));
            Sleep(sleep_time);
        }
    #else
        //limit the fps to 64 ticks for stable step
        if (new_real_time - real_time < fps_interval) {
            timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec = (fps_interval - (new_real_time - real_time)) * 1000;
            nanosleep(&sleep_time, &sleep_time);
        }
    #endif
}

/** @brief calculates fps
  */
void Game::fpsCalc()
{
    //get actual ticks
    ulint new_real_time = ogre->getTimer()->getMilliseconds();

    //smooth displayed fps over time
    fps = 0.8 * fps + 0.2 * (Ogre::Real(1000) / Ogre::Real(new_real_time - real_time));
    real_time = new_real_time;

    //temp, show the fps
    Game::hud->status->setLine(string("$eFPS: ")+intoString(fps), 0, 20, 50);
}

/** @brief terminal error - die
  */
void Game::kill(string a_goodbye)
{
    cout << a_goodbye << endl;
    exit(1);
}

/** @brief quit the game gracefully
  */
void Game::end(string a_goodbye)
{
    state = game_state_closing;
    cout << a_goodbye << endl;
}
