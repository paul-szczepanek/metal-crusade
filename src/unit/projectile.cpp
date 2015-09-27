// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "projectile.h"
#include "game.h"
#include "game_arena.h"
#include "collision_handler.h"
#include "particle_manager.h"
#include "collision.h"
#include "weapon.h"
#include "collision_type.h"
#include "corpus.h"

// how much slower the bullet moves visually
#define VELOCITY_SCALE (0.1)
#define INVERSE_VELOCITY_SCALE (1.0 / (VELOCITY_SCALE))
// when the air resistance start to become negligible (for gameplay purposes, not in real life)
const Real air_resistance_cutoff = 400 * VELOCITY_SCALE;
// grace time for projectiles after spawning so that they don't explode in the owner crusader
const Real grace_period = 10;

Projectile::~Projectile()
{
}

Projectile::Projectile()
{
  reset();
}

void Projectile::reset(Weapon* a_weapon,
                       Corpus* a_corpus)
{
  GracePeriod = grace_period;
  Exploading = false;
  OwnerWeapon = a_weapon;
  Bullet = a_corpus;
  if (OwnerWeapon && Bullet) {
    Lifetime = OwnerWeapon->weapon_design.range;
    // fake speed so that you can see it fly.
    Bullet->Velocity = VELOCITY_SCALE * new_corpus->Direction *
                       a_weapon->weapon_design.muzzle_velocity;
    Bullet->Penetration = OwnerWeapon->weapon_design.penetration;
    Bullet->TotalWeight = OwnerWeapon->weapon_design.projectile_weight;
    Bullet->BallisticDmg = OwnerWeapon->weapon_design.BallisticDmg;
    Bullet->EnergyDmg = OwnerWeapon->weapon_design.EnergyDmg;
    Bullet->HeatDmg = OwnerWeapon->weapon_design.HeatDmg;
    Bullet->CollisionType = collision_type_none;
  }
}

/** @brief resolves collisions
 */
bool Projectile::handleCollision(Collision* a_collision)
{
  if (!Exploading) {
    explode();
  }

  return true;
}

/** @brief sets the exploding state stopping motion and starts the explosion effect if needed
 */
void Projectile::explode()
{
  Exploading = true;

  if (OwnerWeapon->weapon_design.splash_range > 0) {
    Lifetime = OwnerWeapon->weapon_design.splash_range;
    Bullet->BoundingSphere.Radius = 0;
    Game::Particle->createExplosion(Bullet->XYZ, OwnerWeapon->weapon_design.splash_range,
                                    OwnerWeapon->weapon_design.splash_range
                                    / (OwnerWeapon->weapon_design.splash_velocity *
                                       VELOCITY_SCALE),
                                    OwnerWeapon->weapon_design.HeatDmg / 100.0);
  }
}

bool Projectile::update(Real a_dt)
{
  if (GracePeriod < 0) {
    Game::Collision->registerObject(Bullet);
    Bullet->CollisionType = collision_type_impact;
  }

  // kill after range exceeded
  if (Lifetime < 0) {
    if (!Exploading) {
      explode();
    }
  }
  if (Exploading) {
    // does the projectile explode causing splash damage
    if (OwnerWeapon->weapon_design.splash_range > 0) {
      // stop projectile
      Bullet->Velocity = Vector3::ZERO;

      // start splash expansion and degradation
      Real expansion = a_dt * OwnerWeapon->weapon_design.splash_velocity * VELOCITY_SCALE;
      // expand the bounding sphere
      Bullet->BoundingSphere.Radius += expansion;

      // as the ball expands make hp (density) smaller,
      // hp gets below zero if the sphere is larger than max radius of the explosion
      Lifetime = 1 - (Bullet->BoundingSphere.Radius / OwnerWeapon->weapon_design.splash_range);

      // how much of the expolsion damage to apply
      Real coverage = (expansion / OwnerWeapon->weapon_design.splash_range);

      Bullet->BallisticDmg = OwnerWeapon->weapon_design.BallisticDmg
                             * OwnerWeapon->weapon_design.splash_velocity * coverage;
      Bullet->EnergyDmg = OwnerWeapon->weapon_design.EnergyDmg * coverage;
      Bullet->HeatDmg = OwnerWeapon->weapon_design.HeatDmg * coverage;

    } else { // no splash damage
      Lifetime = -1; // kill instantly
    }
  } else {
    // retard motion while the physics weep (air resistance should be Velocity squared)
    if (Vector2(Bullet->Velocity.x, Bullet->Velocity.z).length() > air_resistance_cutoff) {
      Bullet->Velocity -= Direction * OwnerWeapon->weapon_design.muzzle_velocity * VELOCITY_SCALE *
                          a_dt;
    }
    // intentionally wrong because we already retarded the downward motion
    Bullet->Velocity.y = Bullet->Velocity.y - Game::Arena->getGravity() * a_dt *
                         INVERSE_VELOCITY_SCALE;
    // and there's the euluer integration error too and we need it to plunge fast
  }

  // if out of bounds destroy
  if (Lifetime < 0) {
    OwnerWeapon = NULL;
    Bullet->OwnerEntity = NULL;
    Game::Collision->deregisterObject(Bullet);
    Game::destroyModel(Bullet->SceneNode);
    Bullet->SceneNode = NULL;
    return false;
  }

  Vector3 move = Bullet->Velocity * a_dt;
  // count up the length into lifetime
  Lifetime -= move.length();

  return true;
}
