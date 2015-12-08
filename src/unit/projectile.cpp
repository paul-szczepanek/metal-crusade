// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "projectile.h"
#include "game.h"
#include "game_arena.h"
#include "corpus_manager.h"
#include "particle_manager.h"
#include "collision.h"
#include "weapon.h"
#include "collision_type.h"
#include "corpus.h"

// how much slower the bullet moves visually
#define VELOCITY_SCALE ((Real)0.1)

// grace time for projectiles after spawning so that they don't explode in the owner crusader
#define GRACE_PERIOD ((Real)10)

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
  GracePeriod = GRACE_PERIOD;
  Exploading = false;
  OwnerWeapon = a_weapon;
  Bullet = a_corpus;
  if (OwnerWeapon && Bullet) {
    Lifetime = OwnerWeapon->weapon_design.range;
    // fake speed so that you can see it fly.
    Bullet->Velocity = VELOCITY_SCALE * Bullet->Direction *
                       a_weapon->weapon_design.muzzle_velocity;
    Bullet->Penetration = OwnerWeapon->weapon_design.penetration;
    Bullet->Weight = OwnerWeapon->weapon_design.projectile_weight;
    Bullet->BallisticDmg = OwnerWeapon->weapon_design.BallisticDmg;
    Bullet->EnergyDmg = OwnerWeapon->weapon_design.EnergyDmg;
    Bullet->HeatDmg = OwnerWeapon->weapon_design.HeatDmg;
    Bullet->CollisionType = collision_type_none;
  }
}

/** @brief resolves collisions
 */
bool Projectile::validateCollision(Corpus* a_collision)
{
  return true;
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
  // stop projectile
  Bullet->Velocity = Vector3::ZERO;

  if (OwnerWeapon->weapon_design.splash_range > 0) {
    Lifetime = OwnerWeapon->weapon_design.splash_range;
    Bullet->BoundingSphere.Radius = 0;
    Game::Particle->createExplosion(Bullet->XYZ, OwnerWeapon->weapon_design.splash_range,
                                    OwnerWeapon->weapon_design.splash_range
                                    / (OwnerWeapon->weapon_design.splash_velocity),
                                    OwnerWeapon->weapon_design.HeatDmg / 100.0);
  }
}

bool Projectile::update(const Real a_dt)
{
  if (GracePeriod < 0) {
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
      // start splash expansion and degradation
      Real expansion = a_dt * OwnerWeapon->weapon_design.splash_velocity;
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
  }

  // if out of bounds destroy
  if (Lifetime < 0) {
    OwnerWeapon = NULL;
    Game::Corpus->deregisterObject(Bullet);
    destroyModel(Bullet->SceneNode);
    Bullet->SceneNode = NULL;
    return false;
  }

  Vector3 move = Bullet->Velocity * a_dt;
  // count up the length into lifetime
  Lifetime -= move.length();

  return true;
}

const Vector3& Projectile::getXYZ()
{
  return Bullet->XYZ;
}

void Projectile::setXYZ(const Vector3& a_pos)
{
  Bullet->XYZ = a_pos;
}
