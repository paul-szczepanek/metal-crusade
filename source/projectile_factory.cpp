//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "projectile_factory.h"
#include "game.h"
#include "projectile.h"
#include "weapon.h"
#include "query_mask.h"
#include "unit.h"

ProjectileFactory::~ProjectileFactory()
{
    list<Projectile*>::iterator it = projectile_list.begin();
    list<Projectile*>::iterator it_end = projectile_list.end();

    //walk through all projectiles
    for (; it != it_end; ++it) {
        //delete entities
        destroyModel((*it)->scene_node);

        //and then the projectile itself
        delete *it;
    }

    projectile_list.clear();
}

/** @brief updates all projectiles
  */
void ProjectileFactory::updateProjectiles(Ogre::Real a_dt)
{
    //walk through all projectiles and update them
    for (list<Projectile*>::iterator it = projectile_list.begin(); it != projectile_list.end(); ) {
        if ((*it)->update(a_dt) == 1) { //remove if expired
            //destroy the Ogre part
            destroyModel((*it)->scene_node);

            //destroy the projectile
            delete *it;

            //get the iterator to the next item after removal
            it = projectile_list.erase(it);

        } else {
            ++it;
        }
    }
}

/** @brief creates projectiles and adds them to a list
  */
void ProjectileFactory::fireProjectile(Ogre::Vector3 a_pos_xyz, Ogre::Quaternion a_orientation,
                                       Weapon* a_weapon, Unit* a_owner)
{
    string mesh = "bullet"; //temp

    //get unique string from id
    string id_string = getUniqueID()+mesh; //append unique id to name

    //create mesh for the projectile
    Ogre::Entity* projectile_mesh = Game::scene->createEntity(id_string, mesh+".mesh");

    //create root node
    Ogre::SceneNode* projectile_node = Game::scene->getRootSceneNode()->createChildSceneNode();
    projectile_node->attachObject(projectile_mesh); //attach mesh
    projectile_mesh->setQueryFlags(query_mask_projectiles); //mask as projectile

    //create variation to the angle of firing
    Ogre::Radian angle_of_spread(Ogre::Math::RangeRandom(-a_weapon->weapon_spec.spread,
                                 a_weapon->weapon_spec.spread));
    Ogre::Quaternion firing_cone = Ogre::Quaternion(angle_of_spread, Ogre::Vector3::UNIT_Y);

    //TODO: the spread needs to be in a cone, not in a plane
    a_orientation = firing_cone * a_orientation;

    //put the projectile on the list
    projectile_list.push_back(new Projectile(a_pos_xyz, mesh, projectile_node,
                                         a_orientation, a_weapon, a_owner));
}
