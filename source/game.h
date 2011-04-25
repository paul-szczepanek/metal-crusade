//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "main.h"

#ifdef PLATFORM_WIN32
    #include <OgreFrameListener.h>
#else
    #include <OGRE/OgreFrameListener.h>
#endif

#include "game_state.h"

class InputHandler;
class UnitFactory;
class AIFactory;
class Timer;
class Arena;
class Camera;
class CorpusFactory;
class ParticleFactory;
class ProjectileFactory;
class FormationFactory;
class CollisionHandler;
class GameController;
class Hud;
class TextStore;

class Game : public Ogre::FrameListener
{
public:
    ~Game() { };
    static int instantiate();
    static Game* instance();
    void init();

    //the main loop hook
    bool frameRenderingQueued(const Ogre::FrameEvent &evt);

    //game time
    Timer* getTimer() { return timer; };

    //game state
    game_state getGameState() { return state; };
    void setGameState(game_state a_state) { state = a_state; };

    //game controllers
    GameController* getGameController(unsigned int i) { return game_controllers.at(i); };
    //TODO: this is temp!!
    GameController* getGameController(const string& a_name) { return game_controllers.at(0); };

    //I'm going to piggy back on this singleton to globally scope these:
    static Ogre::Root* ogre;
    static Ogre::Viewport* viewport;
    static Ogre::RenderWindow* render_window;
    static Ogre::SceneManager* scene;
    static InputHandler* input_handler;
    static Camera* camera;
    static Arena* arena;
    static Hud* hud;
    static CorpusFactory* corpus_factory;
    static ParticleFactory* particle_factory;
    static ProjectileFactory* projectile_factory;
    static FormationFactory* formation_factory;
    static UnitFactory* unit_factory;
    static AIFactory* ai_factory;
    static TextStore* text;
    static CollisionHandler* collider;
    //some have a strict creation order and you can't destroy one without creating a new one
    //immidiately as the main loop requires all these to be valid
    //since they're not using singleton calls for reason exaplained above
    //(they are all integral but portioned into files for ease of editing)

    //terminate game
    void end(string a_goodbye = "Terminated");
    static void kill(string a_goodbye = "O, untimely death!");

private:
    //singleton
    Game() : game_controllers(16), fps(0) { };
    static Game* inst;

    //main loop
    void input();
    void logic(int dt);
    void fpsCalc();
    void limitFPS();

    //game controllers - one for each active unit - this includes local and remote players
    vector<GameController*> game_controllers;

    game_state state;

    //time
    Timer* timer;
    ulint new_time;
    ulint last_time;
    ulint real_time;
    Ogre::Real fps;
};

#endif // GAME_H_INCLUDED
