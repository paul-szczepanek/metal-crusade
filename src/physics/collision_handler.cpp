// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "game.h"
#include "game_arena.h"
#include "arena_entity.h"
#include "collision_handler.h"
#include "collision.h"
#include "corpus.h"

CollisionHandler::CollisionHandler()
{
  // this should prepare the buckets when and if I get to that
}

CollisionHandler::~CollisionHandler()
{
}

/** @brief put the dynamic object into the collision system
 */
void CollisionHandler::registerObject(Corpus* a_corpus)
{
  RegisteredObjects.push_back(a_corpus);
}

/** @brief remove the dynamic object from the collision system
 */
void CollisionHandler::deregisterObject(Corpus* a_corpus)
{
  RegisteredObjects.remove(a_corpus);
}

/*
   // ignore other projectiles and ignore hits at close range
   if (static_cast<void*>(OwnerWeapon->Owner) == a_colliding_object->OwnerEntity) {
    return false;

   } else if (a_colliding_object->getCollisionType() == collision_type_impact) { // ignore bullets
    return false;

   } else {
    return true;
   }
 */

/** @brief fills a list of potential collisions based on bounding spheres
 */
void CollisionHandler::updatePotentialCollisions()
{
  // clear last frame collisions if any
  PossibleCollisions.clear();

  for (auto c1 : RegisteredObjects) {
    if (c1->CollisionType == collision_type_none) {
      continue;
    }

    c1->updateBoundingSphere();

    // get adjacent cell indexes
    vector<size_t_pair> cell_indexes;
    Game::Arena->getCellIndexesWithinRadius(c1->getCellIndex(), cell_indexes);

    for (const size_t_pair& ci : cell_indexes) {
      // get lists from each cell
      list<Corpus*>& corpus_list = Game::Arena->getCorpusCell(ci);

      for (auto c2 : corpus_list) {
        if (c2->CollisionType == collision_type_none) {
          continue;
        }

        c2->updateBoundingSphere();

        // check bounding spheres
        if (c1->BoundingSphere.intersects(c2->BoundingSphere)) {
          // validate whether the collision is possible
          if (c1->validateCollision(c2)) {
            // make sure we don't record the pair from both sides, impose order on the pair
            collision_pair cp = c1 < c2 ? collision_pair(c1, c2) : collision_pair(c2, c1);
            PossibleCollisions.push_back(cp);
          }
        }
      }
    }

    // collision with the ground
    if (c1->OwnerEntity) {
      Corpus* c2 = c1->OwnerEntity->getGround();
      if (c2) {
        if (c1->BoundingSphere.intersects(c2->BoundingSphere)) {
          PossibleCollisions.push_back(collision_pair(c1, c2));
        }
      }
    }
  }
}

/** @brief check for collision based on individual spheres
 */
void CollisionHandler::evaluatePotentialCollisions()
{
  // go through all list of potential collisions
  for (size_t i = 0, for_size = PossibleCollisions.size(); i < for_size; ++i) {
    Corpus* c1 = PossibleCollisions[i].object1;
    Corpus* c2 = PossibleCollisions[i].object2;

    // get possible collision spheres based on the bounding spheres
    bitset<MAX_NUM_CS> cs_bitset_valid1;
    bitset<MAX_NUM_CS> cs_bitset_valid2;
    c1->pruneCollisionSpheres(c2->BoundingSphere, cs_bitset_valid1);
    c2->pruneCollisionSpheres(c1->BoundingSphere, cs_bitset_valid2);

    if (cs_bitset_valid1.none() || cs_bitset_valid2.none()) {
      return;
    }

    // mark spheres involved in the collision
    bitset<MAX_NUM_CS> cs_bitset1;
    bitset<MAX_NUM_CS> cs_bitset2;
    // mark areas hit
    bitset<MAX_NUM_CS_AREAS> areas_bitset1;
    bitset<MAX_NUM_CS_AREAS> areas_bitset2;

    Real max_depth = 0;

    for (size_t i = 0, for_size = c1->CollisionSpheres.size(); i < for_size; ++i) {
      if (!cs_bitset_valid1[i]) {
        continue;
      }

      Real depth = 0;
      for (size_t j = 0, for_size = c2->CollisionSpheres.size(); j < for_size; ++j) {
        if (!cs_bitset_valid2[j]) {
          continue;
        }

        const Real sphere_depth = c2->CollisionSpheres[j].getDepth(c1->CollisionSpheres[i]);

        if (sphere_depth > HIT_OVERLAP) {
          depth = max(depth, sphere_depth);
          cs_bitset2[j] = 1;
          areas_bitset2[c2->CSAreas[j]] = 1;
        }
      }

      if (depth > HIT_OVERLAP) {
        max_depth = max(max_depth, depth);
        cs_bitset1[i] = 1;
        areas_bitset1[c1->CSAreas[i]] = 1;
      }

    }

    if (cs_bitset1.any()) {
      Collisions.push_back(Collision(c1, c2,
                                     cs_bitset1, cs_bitset2,
                                     areas_bitset1, areas_bitset2,
                                     max_depth));
    }
  }
}

/** @brief produce collisions and evaluate them
 */
void CollisionHandler::update(const Real a_dt)
{
  updatePotentialCollisions();
  evaluatePotentialCollisions();

  // count how many collisions each corpus takes part in and record that in the collisions
  for (Collision col : Collisions) {
    col.findCollisionPlane();
    ++Hits[col.Object[0]];
    ++Hits[col.Object[1]];
  }

  for (Collision col : Collisions) {
    for (const reverse_pairs_t& i : PAIRS) {
      if (Hits[col.Object[i.a]] > 1) {
        // to handle multi body collisions we resolve them in groups multiple times
        col.CollisionGroup[i.a] = &(CollisionGroups[col.Object[i.a]]);
        CollisionGroups[col.Object[i.a]].push_back(&col);
      }
    }
  }

  // resolve each collision
  for (Collision col : Collisions) {
    col.resolve(a_dt);
  }

  // and clear the list
  Collisions.clear();
  Hits.clear();
  CollisionGroups.clear();
}
