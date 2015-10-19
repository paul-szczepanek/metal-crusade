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

#define CAMERA_CLIP_NEAR ((Real)200)
#define CAMERA_CLIP_FAR ((Real)1000)

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
class GameHud;

class Game
  : public Ogre::FrameListener
{
public:
  ~Game();

  // the main loop hook
  bool frameRenderingQueued(const Ogre::FrameEvent &a_evt);

  static int init(bool a_debug_on);
  static Game* instance();
  void run();

  static const string& getVersion();
  static string getUniqueID();

  // game time
  static Timer* getTimer();

  // game state
  static game_state getGameState();
  static void setGameState(game_state a_state);

  // game controllers
  static GameController* getGameController(size_t i);
  // TODO: this is temp!!
  static GameController* getGameController(const string& /*aName*/);

  static void log(const string& a_text);

  // terminate game
  static void end(string a_goodbye = "Terminated");
  static void kill(string a_goodbye = "Terminal Error");

private:
  Game();

  static void enterArena();
  static void exitArena();

  static void input();
  static void logic(ulint a_ticks);
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
  static Timer* GameTimer;
  static GameHud* Hud;

  static Real Delta;

private:
  // game controllers - one for each active unit - this includes local and remote players
  static vector<GameController*> Controllers;

  static game_state State;

  // time
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
inline void Game::end(string a_goodbye)
{
  State = game_state_closing;
  cout << a_goodbye << endl;
}

inline Timer* Game::getTimer()
{
  return GameTimer;
}

inline game_state Game::getGameState()
{
  return State;
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

/** @brief clears the Ogre objects
 */
inline void destroyModel(Ogre::SceneNode* a_scene_node)
{
  // destroy attached entities
  Ogre::SceneNode::ObjectIterator it = a_scene_node->getAttachedObjectIterator();
  while (it.hasMoreElements()) {
    Ogre::MovableObject* movable_object = static_cast<Ogre::MovableObject*>(it.getNext());
    a_scene_node->getCreator()->destroyMovableObject(movable_object);
  }

  // destroy children if any
  Ogre::SceneNode::ChildNodeIterator it2 = a_scene_node->getChildIterator();
  while (it2.hasMoreElements()) {
    Ogre::SceneNode* child_node = static_cast<Ogre::SceneNode*>(it2.getNext());
    destroyModel(child_node);
  }

  // at last remove the scene node
  Game::Scene->destroySceneNode(a_scene_node);
}

#endif // GAME_H_INCLUDED
