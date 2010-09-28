//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "main.h"
#include <OGRE/OgreFrameListener.h>
#include "game_state.h"

class InputHandler;
class UnitFactory;
class AIFactory;
class Timer;
class Arena;
class Camera;
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

    //becuase I am a lazy fuck and becase it saves on the singleton call overhead
    //I'm going to piggy back on this singleton to globally scope these:
    static Ogre::Root* ogre;
    static Ogre::Viewport* viewport;
    static Ogre::RenderWindow* render_window;
    static Ogre::SceneManager* scene;
    static InputHandler* input_handler;
    static Camera* camera;
    static Arena* arena;
    static Hud* hud;
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

    //helper functions
    static int stringIntoInt(string& a_string);
    static Ogre::Real stringIntoReal(string& a_string);
    static string intIntoString(int a_integer);
    static string realIntoString(Ogre::Real a_real, usint a_precision = 2);
    bool static take(bool& do_once);

    //terminate game
    void end(string a_goodbye = "Terminated");
    static void kill(string a_goodbye = "O, untimely death!");

    //temp debug
    void debug();
    vector<string> debug_lines;

private:
    //singleton
    Game() : debug_lines(20), game_controllers(16), fps(0) { };
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
    ulint debug_time;
    ulint new_time;
    ulint last_time;
    ulint real_time;
    Ogre::Real fps;
};

inline bool Game::take(bool& do_once)
{
    if (do_once) {
        do_once = false;
        return true;
    }
    return false;
}

inline int Game::stringIntoInt(string& a_string)
{
    stringstream stream;
    int integer;
    stream << a_string;
    stream >> integer;
    return integer;
}

inline string Game::intIntoString(int a_integer)
{
    stringstream stream;
    string converted_string;
    stream << a_integer;
    stream >> converted_string;
    return converted_string;
}

inline Ogre::Real Game::stringIntoReal(string& a_string)
{
    stringstream stream;
    Ogre::Real real;
    stream << a_string;
    stream >> real;
    return real;
}

inline string Game::realIntoString(Ogre::Real a_real, usint a_precision)
{
    stringstream stream;
    string converted_string;
    stream << setprecision(a_precision) << a_real;
    stream >> converted_string;
    return converted_string;
}

#endif // GAME_H_INCLUDED
