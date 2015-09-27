// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "main.h"

#ifdef PLATFORM_WIN32
#include <OgreFrameListener.h>
#else
#include <OGRE/OgreFrameListener.h>
#endif

#include "game_state.h"

class Timer;
class GameController;
class InputHandler;
class TextStore;
class GameCamera;
class CorpusManager;
class FormationManager;
class FactionManager;
class GameArena;
class AIManager;
class ParticleManager;
class UnitFactory;
class BuildingFactory;
class ProjectileFactory;
class CollisionHandler;

class Game
  : public Ogre::FrameListener
{
public:
  ~Game();

  static int init(bool aDebugOn);
  static Game* instance();
  void run();

  static const string& getVersion();
  static string getUniqueID();

  // the main loop hook
  bool frameRenderingQueued(const Ogre::FrameEvent &aEvt);

  // game time
  static Timer* getTimer();

  // game state
  static game_state getGameState();
  static void setGameState(game_state aState);

  // game controllers
  static GameController* getGameController(size_t i);
  // TODO: this is temp!!
  static GameController* getGameController(const string& /*aName*/);

  static void log(const string& aText);

  static void destroyModel(Ogre::SceneNode* a_scene_node);

  // terminate game
  static void end(string aGoodbye = "Terminated");
  static void kill(string aGoodbye = "Terminal Error");

private:
  Game();

  static void enterArena();
  static void exitArena();

  static void input();
  static void logic(int aDTicks);
  static void fpsCalc();
  static void limitFPS();

public:
  static Ogre::Root* OgreRoot;
  static Ogre::Viewport* OgreViewport;
  static Ogre::RenderWindow* OgreWindow;
  static Ogre::SceneManager* Scene;

  static InputHandler* Input;
  static TextStore* Text;
  static GameCamera* Camera;
  static CorpusManager* Corpus;
  static FormationManager* Formation;
  static FactionManager* Faction;
  static GameArena* Arena;
  static AIManager* AI;
  static ParticleManager* Particle;
  static UnitFactory* Unit;
  static BuildingFactory* Building;
  static ProjectileFactory* Projectile;
  static CollisionHandler* Collision;

  static Real Delta;

private:
  // game controllers - one for each active unit - this includes local and remote players
  static vector<GameController*> Controllers;

  static game_state State;

  // time
  static Timer* GameTimer;
  static ulint NewTime;
  static ulint LastTime;
  static ulint RealTime;
  static Real Fps;

  static size_t UniqueId;

public:
  static bool DebugMode;
};

/** @brief quit the game gracefully
 */
inline void Game::end(string aGoodbye)
{
  State = game_state_closing;
  cout << aGoodbye << endl;
}

inline Timer* Game::getTimer()
{
  return GameTimer;
}

inline game_state Game::getGameState()
{
  return State;
}

inline void Game::setGameState(game_state aState)
{
  State = aState;
}

inline GameController* Game::getGameController(size_t i)
{
  return Controllers[i];
}

inline GameController* Game::getGameController(const string& /*aName*/)
{
  return Controllers[0];
}

inline string Game::getUniqueID()
{
  string id_string;
  stringstream stream;
  stream << setfill('0') << setw(10) << ++UniqueId; // format id string to 0000000001
  stream >> id_string;
  return id_string;
}

#endif // GAME_H_INCLUDED
