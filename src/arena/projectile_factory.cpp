// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "projectile_factory.h"
#include "game.h"
#include "projectile.h"
#include "weapon.h"
#include "query_mask.h"
#include "unit.h"

ProjectileFactory::ProjectileFactory()
  : Projectiles(32),
  Corpuses(32)
{
}

ProjectileFactory::~ProjectileFactory()
{
}

/** @brief updates all projectiles
 */
void ProjectileFactory::update(Real a_dt)
{
  // walk through all projectiles and update them
  for (auto it = ActiveList.begin(); it != ActiveList.end(); ) {
    (*it)->update(a_dt);

    if ((*it)->OwnerEntity == NULL) { // remove if expired
      // get the iterator to the next item after removal
      it = ActiveList.erase(it);
    } else {
      ++it;
    }
  }
}

/** @brief getFreeProjectile
 *
 * @todo: document this function
 */
Projectile* ProjectileFactory::getFreeProjectile()
{
  for (size_t i = 0; i < Projectiles.size(); ++i) {
    lastProjectileIdx = (lastProjectileIdx + 1) % Projectiles.size();
    if (Projectiles[lastProjectileIdx]->Expired) {
      break;
    }

    if (i > Projectiles.size() / 4) {
      // if we waster too much time looking for a free one, double the size and return a new one
      lastProjectileIdx = Projectiles.size();
      Projectiles.resize(Projectiles.size() * 2);
      break;
    }
  }

  return Projectiles[lastProjectileIdx];
}

/** @brief getFreeCorpus
 *
 * @todo: document this function
 */
Corpus* ProjectileFactory::getFreeCorpus()
{
  for (size_t i = 0; i < Corpuses.size(); ++i) {
    lastCorpusIdx = (lastCorpusIdx + 1) % Corpuses.size();
    if (Corpuses[lastCorpusIdx]->OwnerEntity == NULL) {
      break;
    }

    if (i > Corpuses.size() / 4) {
      // if we waster too much time looking for a free one, double the size and return a new one
      lastCorpusIdx = Corpuses.size();
      Corpuses.resize(Corpuses.size() * 2);
      break;
    }
  }

  return Corpuses[lastCorpusIdx];
}

/** @brief creates projectiles and adds them to a list
 */
void ProjectileFactory::fireProjectile(Vector3    a_pos_xyz,
                                       Quaternion a_orientation,
                                       Weapon*    a_weapon)
{
  string mesh = "bullet"; // temp

  // get unique string from id
  string id_string = Game::getUniqueID() + mesh;

  // create mesh for the projectile
  Ogre::Entity* projectile_mesh = Game::scene->createEntity(id_string, mesh + ".mesh");

  // create root node
  Ogre::SceneNode* projectile_node = Game::scene->getRootSceneNode()->createChildSceneNode();
  projectile_node->attachObject(projectile_mesh); // attach mesh
  projectile_mesh->setQueryFlags(query_mask_projectiles); // mask as projectile

  Projectile* new_projectile = getFreeProjectile();
  new_projectile->setName(id_string);
  new_projectile->NumCorpuses = 1;
  new_projectile->weapon = a_weapon;
  new_projectile->Owner = a_weapon->Owner;

  Corpus* new_corpus = getFreeCorpus();
  new_corpus->setOwner(new_projectile);
  new_corpus->setSceneNode(projectile_node);
  new_corpus->setXYZ(a_pos_xyz);
  new_corpus->setOrientation(a_orientation);
  new_corpus->loadCollisionSpheres(mesh);

  // put the projectile on the list
  ActiveList.push_back(new_corpus);
}
