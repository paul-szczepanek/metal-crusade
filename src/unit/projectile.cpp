// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "projectile.h"
#include "game.h"
#include "game_arena.h"
#include "collision_handler.h"
#include "particle_manager.h"
#include "collision.h"
#include "weapon.h"

// how much slower the bullet moves visually
const Real velocity_scale = 0.1;
const Real inverse_velocity_scale = 10;
// when the air resistance start to become negligible (for gameplay purposes, not in real life)
const Real air_resistance_cutoff = 400 * velocity_scale;
// grace time for projectiles after spawning so that they don't explode in the owner crusader
const Real grace_period = 10;

Projectile::Projectile(Vector3          a_pos_xyz,
                       const string&    a_unit_name,
                       Ogre::SceneNode* a_scene_node,
                       Quaternion       a_orientation,
                       Weapon*          a_weapon,
                       Corpus*          a_owner)
  : weapon(a_weapon),
  NumCorpuses(0),
  lifetime(0),
  exploading(false),
  coverage(1),
  velocity_dmg_multiplier(1)
{
  // read properties from weapon spec
  penetration = weapon->weapon_design.penetration;
  total_weight = weapon->weapon_design.projectile_weight;

  // all projectiles die on collision
  collision = collision_type_impact;

  // fake speed so that you can see it fly.
  velocity = direction * velocity_scale * weapon->weapon_design.muzzle_velocity;

  // projectiles don't need one, they are instantly active
  controller_active = true;
}

/** @brief interface for collision resolution when the other object is taking damage
 */
Real Projectile::getBallisticDmg()
{
  return weapon->weapon_design.ballistic_dmg * coverage * velocity_dmg_multiplier;
}

Real Projectile::getEnergyDmg()
{
  return weapon->weapon_design.energy_dmg * coverage;
}

Real Projectile::getHeatDmg()
{
  return weapon->weapon_design.heat_dmg * coverage;
}

/** @brief check if the collision can happen and prepares the projectile to handle it
 */
bool Projectile::validateCollision(Corpus* a_colliding_object)
{
  // ignore other projectiles and ignore hits at close range
  if (lifetime < grace_period && owner == a_colliding_object) {
    return false;

  } else if (a_colliding_object->getCollisionType() == collision_type_impact) { // ignore bullets
    return false;

  } else {
    if (exploading) { // if the proejctile is already exploading turn the multiplier off
      velocity_dmg_multiplier = 1;

    } else { // if it's the first impact and not exploading yet
      // make the damage inversly proportional to the loss of velocity after firing
      velocity_dmg_multiplier
        = velocity.length() / (weapon->weapon_design.muzzle_velocity * velocity_scale);
    }
    return true;
  }
}

/** @brief resolves collisions
 */
int Projectile::handleCollision(Collision* a_collision)
{
  // explode and apply velocity
  velocity = a_collision->getVelocity();

  if (!exploading) {
    explode();
  }

  return 0;
}

/** @brief sets the exploding state stopping motion and starts the explosion effect if needed
 */
void Projectile::explode()
{
  exploading = true;

  lifetime = grace_period;

  if (weapon->weapon_design.splash_range > 0) {
    Game::particle_factory->createExplosion(pos_xyz, weapon->weapon_design.splash_range,
                                            weapon->weapon_design.splash_range
                                            / (weapon->weapon_design.splash_velocity *
                                               velocity_scale),
                                            weapon->weapon_design.heat_dmg / 100.0);
  }
}

int Projectile::update()
{
  // kill after terrain hit
  if (pos_xyz.y < Game::Arena->getHeight(pos_xyz.x, pos_xyz.z) // kill when ground hit
      || lifetime > weapon->weapon_design.range) { // kill after range exceeded
    if (!exploading) {
      explode();
    }
  }
  if (exploading) {
    // does the projectile explode causing splash damage
    if (weapon->weapon_design.splash_range > 0) {
      // stop projectile
      velocity = Vector3::ZERO;

      // start splash expansion and degradation
      Real expansion = dt * weapon->weapon_design.splash_velocity * velocity_scale;
      // expand the bounding sphere
      bounding_sphere.radius += expansion;

      // as the ball expands make hp (density) smaller,
      // hp gets below zero if the sphere is larger than max radius of the explosion
      core_integrity = 1 - (bounding_sphere.radius / weapon->weapon_design.splash_range);

      // how much of the expolsion damage to apply
      coverage = (expansion / weapon->weapon_design.splash_range);

    } else { // no splash damage
      core_integrity = 0; // kill instantly
    }
  } else {
    // retard motion while the physics weep (air resistance should be velocity squared)
    if (Vector2(velocity.x, velocity.z).length() > air_resistance_cutoff) {
      velocity -= direction * weapon->weapon_design.muzzle_velocity * velocity_scale * dt;
    }
    // intentionally wrong because we already retarded the downward motion
    velocity.y = velocity.y - Game::Arena->getGravity() * dt * inverse_velocity_scale;
    // and there's the euluer integration error too and we need it to plunge fast
  }

  if (core_integrity <= 0) { // kill on hp 0
    return 1;
  }

  // move and save the difference of positions into move
  move = velocity * dt;
  pos_xyz += move;
  direction = move;
  direction.normalise();

  // count up the length into lifetime
  lifetime += move.length();

  // if out of bounds destroy
  if (out_of_bounds) {
    return 1;
  }

  return 0;
}
