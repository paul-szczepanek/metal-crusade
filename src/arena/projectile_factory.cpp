// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "projectile_factory.h"
#include "game.h"
#include "projectile.h"
#include "weapon.h"
#include "query_mask.h"
#include "unit.h"
#include "game_arena.h"
#include "corpus_manager.h"

ProjectileFactory::ProjectileFactory()
  : Projectiles(32),
  Corpuses(32)
{
}

ProjectileFactory::~ProjectileFactory()
{
}

/** @brief getFreeProjectile
 *
 * @todo: document this function
 */
Projectile* ProjectileFactory::getFreeProjectile()
{
  for (size_t i = 0; i < Projectiles.size(); ++i) {
    LastProjectileIdx = (LastProjectileIdx + 1) % Projectiles.size();
    if (Projectiles[LastProjectileIdx].OwnerWeapon == NULL) {
      break;
    }

    if (i > Projectiles.size() / 4) {
      // if we waster too much time looking for a free one, double the size and return a new one
      LastProjectileIdx = Projectiles.size();
      Projectiles.resize(Projectiles.size() * 2);
      break;
    }
  }

  return &Projectiles[LastProjectileIdx];
}

/** @brief getFreeCorpus
 *
 * @todo: document this function
 */
Corpus* ProjectileFactory::getFreeCorpus()
{
  for (size_t i = 0; i < Corpuses.size(); ++i) {
    LastCorpusIdx = (LastCorpusIdx + 1) % Corpuses.size();
    if (!Corpuses[LastCorpusIdx].OnArena) {
      break;
    }

    if (i > Corpuses.size() / 4) {
      // if we waster too much time looking for a free one, double the size and return a new one
      LastCorpusIdx = Corpuses.size();
      Corpuses.resize(Corpuses.size() * 2);
      break;
    }
  }

  return &Corpuses[LastCorpusIdx];
}

/** @brief creates projectiles and adds them to a list
 */
Projectile* ProjectileFactory::spawnProjectile(Vector3    a_pos_xyz,
                                               Quaternion a_orientation,
                                               Weapon*    a_weapon)
{
  string mesh = "bullet"; // temp

  // get unique string from id
  string id_string = Game::getUniqueID() + mesh;

  // create mesh for the projectile
  Ogre::Entity* projectile_mesh = Game::Scene->createEntity(id_string, mesh + ".mesh");

  // create root node
  Ogre::SceneNode* projectile_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  projectile_node->attachObject(projectile_mesh); // attach mesh
  projectile_mesh->setQueryFlags(query_mask_projectiles); // mask as projectile

  Corpus* new_corpus = getFreeCorpus();
  new_corpus->setOwner(a_weapon->Owner);
  new_corpus->setSceneNode(projectile_node);
  new_corpus->XYZ = a_pos_xyz;
  new_corpus->setOrientation(a_orientation);
  new_corpus->loadCollision(mesh);
  new_corpus->CollisionType = collision_type_none;
  Game::Corpus->registerDynamicObject(new_corpus);

  Projectile* new_projectile = getFreeProjectile();
  new_projectile->reset(a_weapon, new_corpus);
  new_projectile->setName(id_string);

  return new_projectile;
}
