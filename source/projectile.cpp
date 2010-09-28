//Copyright 2010 Paul Szczepanek. Code released under GPLv3

#include "projectile.h"
#include "game.h"
#include "arena.h"
#include "collision_handler.h"
#include "collision.h"
#include "weapon.h"

//how muc slower the bullet moves visually
const Ogre::Real velocity_scale = 0.1;
const Ogre::Real inverse_velocity_scale = 10;
//when the air resistance start to become negligable (for gameplay purposes, not in real life)
const Ogre::Real air_resistance_cutoff = 400 * velocity_scale;

Projectile::Projectile(Ogre::Vector3 a_pos_xyz, const string& a_unit_name,
                       Ogre::SceneNode* a_scene_node, Ogre::Quaternion a_orientation,
                       Weapon* a_weapon, Corpus* a_owner)
    : Mobilis::Mobilis(a_pos_xyz, a_unit_name, a_scene_node, a_orientation),
    owner(a_owner), weapon(a_weapon), lifetime(0), exploading(false), coverage(1),
    velocity_dmg_multiplier(1)
{
    //read properties from weapon spec
    penetration = weapon->weapon_spec.penetration;
    total_weight = weapon->weapon_spec.projectile_weight;

    //all projectiles die on collision
    collision = collision_type_impact;

    //fake speed so that you can see it fly.
    velocity = direction * velocity_scale * weapon->weapon_spec.muzzle_velocity;

    //projectiles don't need one, they are instantly active
    controller_active = true;
}

Ogre::Real Projectile::getBallisticDmg()
{
    return weapon->weapon_spec.ballistic_dmg * coverage * velocity_dmg_multiplier;
}

Ogre::Real Projectile::getEnergyDmg()
{
    return weapon->weapon_spec.energy_dmg * coverage;
}

Ogre::Real Projectile::getHeatDmg()
{
    return weapon->weapon_spec.heat_dmg * coverage;
}

bool Projectile::validateCollision(Corpus* a_colliding_object)
{
    //ignore other projectiles and ignore hits at close range
    if (lifetime < 10 && owner == a_colliding_object) {
        return false;

    } else if (a_colliding_object->getCollisionType() == collision_type_impact) { //ignore bullets
        return false;

    } else {
        if (exploading) { //if the proejctile is already exploading turn the multiplier off
            velocity_dmg_multiplier = 1;

        } else { //if it's the first impact and not exploading yet
            //make the damage inversly proportional to the loss of velocity after firing
            velocity_dmg_multiplier
                = velocity.length() / (weapon->weapon_spec.muzzle_velocity * velocity_scale);
        }
        return true;
    }
}

int Projectile::handleCollision(Collision* a_collision)
{
    //explode and apply velocity
    exploading = true;
    velocity = a_collision->getVelocity();

    return 0;
}

int Projectile::updateController()
{
    //kill after terrain hit
    if (pos_xyz.y < Game::arena->getHeight(pos_xyz.x, pos_xyz.z) //kill when ground hit
        || lifetime > weapon->weapon_spec.range) //kill after range exceeded
        exploading = true;

    //TODO: on extermely low framerate this could destroy the splash without causing any collisions
    if (exploading) {
        //does the projectile explode causing splash damage
        if (weapon->weapon_spec.splash_range > 0) {
            //stop projectile
            velocity = Ogre::Vector3::ZERO;

            //start splash expansion and degradation
            Ogre::Real expansion = dt * weapon->weapon_spec.splash_velocity;
            //expand the bounding sphere
            bounding_sphere.radius += expansion;

            //as the ball expands make hp (density) smaller,
            //hp gets below zero if the sphere is larger than max radius of the explosion
            core_integrity = 1 - (bounding_sphere.radius / weapon->weapon_spec.splash_range);

            //how much of the expolsion damage to apply
            coverage = (expansion / weapon->weapon_spec.splash_range);

        } else { //no splash damage
            core_integrity = 0; //kill instantly
        }
    }

    if (core_integrity <= 0) { //kill on hp 0
        return 1;
    }

    //retard motion while the physics weep (air resistance should be velocity squared)
    if (Ogre::Vector2(velocity.x, velocity.z).length() > air_resistance_cutoff) {
        velocity -= direction * weapon->weapon_spec.muzzle_velocity * velocity_scale * dt;
    }
    //intentionally wrong because we already retarded the downward motion
    velocity.y = velocity.y - Game::arena->getGravity() * dt * inverse_velocity_scale;
    //and there's the euluer integration error too and we need it to plunge fast

    //move and save the difference of positions into move
    move = velocity * dt;
    pos_xyz += move;
    direction = move;
    direction.normalise();

    //count up the length into lifetime
    lifetime += move.length();

    //if out of bounds destroy
    if (out_of_bounds) {
        return 1;
    }

    return 0;
}
