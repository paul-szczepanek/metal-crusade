//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "crusader.h"
#include "game.h"
#include "arena.h"
#include "hud.h"
#include "files_handler.h"
#include "collision_handler.h"
#include "particle_factory.h"
#include "particle_effect_step_dust.h"
#include "weapon.h"
#include "game_controller.h"
#include "query_mask.h"
#include "collision.h"
#include "animation.h"
#include "log_computer.h"
#include "status_computer.h"
#include "radar_computer.h"
#include "text_store.h"

/** gets positions for all the panels used to mount weapons
  * //TEMP!!!
  * @todo: read from mesh instead
  */
void Crusader::positionWeapons(vector<Ogre::Vector3>& panel_positions, vector<usint>& slots_used)
{
    //fake - position should be read from mesh
    vector<Ogre::Vector3> positions(chasis.num_of_parts, Ogre::Vector3(0, 0, 0));
    positions[0] = (Ogre::Vector3(0, -0.9, 3));
    positions[1] = (Ogre::Vector3(2, -0.9, 3));
    positions[2] = (Ogre::Vector3(-2, -0.9, 3));
    positions[3] = (Ogre::Vector3(3.8, -0.9, 3));
    positions[4] = (Ogre::Vector3(-3.8, -0.9, 3));
    positions[5] = (Ogre::Vector3(3.8, -0.9, 3));
    positions[6] = (Ogre::Vector3(-3.8, -0.9, 3));
    //positions[7] = (Ogre::Vector3(3.8, -1.9, 3)); //legs unused for now at least
    //positions[8] = (Ogre::Vector3(-3.8, -1.9, 3));

    //translate panel positions to weapon positions
    usint k = 0;
    for (usint j = 0; j < chasis.num_of_parts; ++j) {
        slots_used[j] = k;
        for (usint i = 0; i < chasis.panels[j]; ++i) {
            panel_positions[k] = positions[j];
            ++k;
        }
    }
}

Crusader::~Crusader()
{
    for (usint i = 0, for_size = design.weapons.size(); i < for_size; ++i) {
        delete weapons[i];
    }

    delete terrain_ray;
    delete terrain_ray_query;

    delete animation;

    delete radar;
};

/** This reads the Crusader definition from a file and creates one
  * @todo: add more internal types and read things from the mesh
  */
Crusader::Crusader(Ogre::Vector3 a_pos_xyz, const string& a_unit_name,
                   Ogre::SceneNode* a_scene_node, Ogre::Quaternion a_orientation,
                   crusader_design_t a_design, crusader_engine_t a_engine,
                   crusader_drive_t a_drive, crusader_chasis_t a_chasis)
    : Unit(a_pos_xyz, a_unit_name, a_scene_node, a_orientation), //chain constructors
    weapons_operational(false), current_group(0), current_weapon(0),
    shock_damage_old(Ogre::Vector3::ZERO), shock_damage_new(Ogre::Vector3::ZERO),
    design(a_design), engine(a_engine), drive(a_drive), chasis(a_chasis),
    throttle(0), angular_momentum_top(0), crusader_height(0), coolant_level(100)
{
    //for battering, should be part of design struct
    penetration = 0.1;
    collision = collision_type_soft;

    //set temperature to ambient temp
    core_temperature = surface_temperature;
    engine_temperature = surface_temperature;

    //resize vectors to fit the num of parts and areas
    structure.resize(chasis.num_of_parts, 0);
    armour.resize(chasis.num_of_areas, 0);

    torso_orientation = orientation; //at start align torso with drive
    torso_direction = orientation * torso_orientation * Ogre::Vector3(0, 0, 1);
    torso_node = static_cast<Ogre::SceneNode*>(scene_node->getChild(0));
    //center of mech is at the top of the drive so we need the highest mesh attached to the drive
    Ogre::SceneNode::ObjectIterator it = scene_node->getAttachedObjectIterator();
    while (it.hasMoreElements()) {
        Ogre::Real y = it.getNext()->getBoundingBox().getSize().y;
        if (y > crusader_height)
            crusader_height = y;
    }

    //positioning weapons
    unsigned int total_panels = 0; //count panels
    for (usint i = 0; i < chasis.num_of_parts; ++i) {
        total_panels += chasis.panels[i];
    }
    vector<usint> slots_used(chasis.num_of_parts, 0);
    vector<Ogre::Vector3> panel_positions(total_panels);

    //get positions of panels and tranlate to weapon positions
    positionWeapons(panel_positions, slots_used);

    //create weapons at those mount points
    for (usint i = 0, for_size = design.weapons.size(); i < for_size; ++i) {
        weapons.push_back(new Weapon(design.weapons[i], this,
                                     panel_positions[slots_used[design.weapons_placement[i]]],
                                     //if extra ammo vector exists at this point
                                     (i < design.weapons_extra_ammo.size())?
                                     design.weapons_extra_ammo[i] //use it
                                     : 0)); //oterwise assume no extra ammo

        //also adds extra ammo if available for that weapon
        slots_used[design.weapons_placement[i]] += weapons[i]->weapon_design.panels;
    }

    //add heatsinks
    heatsinks = count(design.internals.begin(), design.internals.end(), internal_type_heatsink);

    //add internal_strucuture elements to base structure
    for (usint i = 0; i < chasis.num_of_parts; ++i) {
        structure[i] = chasis.structure_base;

        //go through all internals
        for (usint j = 0, for_size = design.internals.size(); j < for_size; ++j) {
            //check if it's a structure and in the right body part
            if (design.internals[j] == internal_type_structure
                    && design.internals_placement[j] == i) {
                //add it to the strucutre of that body part
                structure[i] += 1;
            }
        }
    }

    //assign armour from design
    for (usint i = 0; i < chasis.num_of_areas; ++i) {
        assert(chasis.surface_area[i] > 0);
        armour[i] = design.armour_placement[i]; // temp? / chasis.surface_area[i];
    }

    //get armour characteristics
    armour_structure = armour_type_to_structure[static_cast<unsigned int>(design.armour)];
    armour_ballistic = armour_type_to_ballistic[static_cast<unsigned int>(design.armour)];
    armour_conductivity = armour_type_to_conductivity[static_cast<unsigned int>(design.armour)];
    armour_generated_heat = armour_type_to_generated_heat[static_cast<unsigned int>(design.armour)];

    //TODO: load equipemnt
    //design.equipment;

    //TODO: load coolant from design
    coolant = 4;//temp

    //after loading all the weapons etc. recalculate weight
    recalculateWeight();

    //temp and obselete anyway, clamping will be done against a heightmap and delegated to arena
    //ray query for terrain clamping
    terrain_ray = new Ogre::Ray();
    terrain_ray_query = new Ogre::DefaultRaySceneQuery(Game::scene);
    terrain_ray_query->setSortByDistance(true);
    terrain_ray_query->setQueryMask(query_mask_pointer_floor);

    //get animations
    animation = new Animation(scene_node);

    //creat the radar
    radar = new RadarComputer(design.radar, this);

    //assign dust emmitter for walking on surfaces producing particles
    Ogre::SceneNode* step_dust_node = scene_node->createChildSceneNode();
    //put the scene node at ground level
    step_dust_node->setPosition(Ogre::Vector3(0, -crusader_height, 0));
    step_dust = static_cast<ParticleEffectStepDust*>
                    (Game::particle_factory->createStepDust(step_dust_node));
}

/** @brief resolves collision including damage and physics
  */
int Crusader::handleCollision(Collision* a_collision)
{
    //apply the new velocity (cushion spikes on the cheap)
    //this is wrong on many levels, done completely ouside of physics logic and only works
    //because collisions revert illegal positions
    velocity = (velocity * 0.1 + a_collision->getVelocity() * 0.85); //doesn't add up to 1
    //TODO: collisions need doing properly
    //this is just compensating for the way it's bollocked atm

    //get affected body parts
    bitset<num_of_body_areas> body_areas_bitset;
    vector<crusader_area::body_area> body_areas_hit;
    vector<usint> sphere_indices = a_collision->getCollisionSphereIndices();

    //get all parts involved in collision
    for (usint i = 0, for_size = sphere_indices.size(); i < for_size; ++i) {
        body_areas_bitset[cs_areas[sphere_indices[i]]] = 1;
    }

    //translate bodyset into a vector of enum body area
    for (usint i = 0; (i < num_of_body_areas) && body_areas_bitset.any(); ++i) {
        if (body_areas_bitset[i]) {
            body_areas_bitset[i] = 0;
            body_areas_hit.push_back(static_cast<crusader_area::body_area>(i));
        }
    }

    //spread the damage evenly among hit parts this is mostly for splash damage
    Ogre::Real damage_spread = 1.0 / body_areas_hit.size();

    //move this to a separate function
    Corpus* hit = a_collision->getCollidingObject();

    //damage
    Ogre::Real ballistic_dmg = hit->getBallisticDmg() * damage_spread;
    Ogre::Real energy_dmg = hit->getEnergyDmg() * damage_spread;
    Ogre::Real heat_dmg = hit->getHeatDmg() * damage_spread;

    if (damage_spread > 1) {
        //
        heat_dmg = heat_dmg * 1;
        if (heat_dmg > 0) {
            //break
            heat_dmg *= 1;
        }
    }

    //summed for all the parts
    Ogre::Real total_conductivity = 0;
    Ogre::Real heat = 0;

    for (usint i = 0, for_size = body_areas_hit.size(); i < for_size; ++i) {
        //armour with penetration applied
        Ogre::Real effective_armour = armour[body_areas_hit[i]] / (hit->getPenetration() + 1);

        //ballistic damage
        Ogre::Real damage = ballistic_dmg / (effective_armour * armour_ballistic + 1);

        //energy damage
        damage += energy_dmg / (effective_armour * armour_conductivity + 1);

        //heat from damage
        Ogre::Real local_armour_conductivity = armour_conductivity;// / (effective_armour + 1);
        heat += heat_dmg * local_armour_conductivity;

        //heat generated from armour reaction
        heat += (ballistic_dmg + energy_dmg) * armour_generated_heat;

        //armour damage proportional to coverage
        Ogre::Real armour_damage = damage * (armour[body_areas_hit[i]]
                                             / design.armour_placement[body_areas_hit[i]]);
        armour[body_areas_hit[i]] -= (armour_damage / armour_structure);
        //cap armour at 0
        if (armour[body_areas_hit[i]] < 0) {
            armour[body_areas_hit[i]] = 0;
        }

        //all the rest goes to structure
        damage -= armour_damage;

        //if damage was greater than what armour could stop
        if (damage > 0) {
            //translate body area into body part
            crusader_area::body_area area_hit = body_areas_hit[i];
            crusader_part::body_part part_hit;

            //areas at the back translated to the same part as the front
            if (area_hit == crusader_area::torso_back) {
                part_hit = crusader_part::torso;

            } else if (area_hit == crusader_area::torso_left_back) {
                part_hit = crusader_part::torso_left;

            } else if (area_hit == crusader_area::torso_right_back) {
                part_hit = crusader_part::torso_right;

            } else {
                part_hit = static_cast<crusader_part::body_part>(area_hit);
            }

            //damage the part's structure
            structure[part_hit] -= damage;

            //if goes below 0 move the damage to integrity //TODO: destroying limbs
            if (structure[part_hit] < 0) {
                //subtract negative structure from core integrity
                core_integrity += structure[part_hit] / chasis.weight; //scale from weight
                structure[part_hit] = 0;
            }
        }

        //counts it up to get and average
        total_conductivity += damage_spread * local_armour_conductivity;
    }

    //conductivity handed to collision for heat transfer
    a_collision->setHitConductivity(total_conductivity);

    //apply heat damage (weight used for scaling as above)
    surface_temperature += heat / chasis.weight;

    return 0;
}

/** damage suffered from G forces
  */
void Crusader::shockDamage()
{
    //to make sure dt is sane (1 - span) is > 0
    //Ogre::Real span = min(dt, Ogre::Real(0.1)); //already guaranteed by timer

    //average over span
    shock_damage_new = shock_damage_new * (1 - dt * 10) + velocity * dt * 10;
    //average over a two spans
    shock_damage_old = shock_damage_old * (1 - dt) + velocity * dt;

    //get the change in velocity
    Ogre::Real shock = (shock_damage_new - shock_damage_old).length();

    //ignore 0.6G force changes as they would likely be tiny anyway
    if (shock > 6) {
        //damage multipilied by weight / scale ratio
        Ogre::Real kinetic_damage = shock * (total_weight / chasis.weight);

        //with zero damage at 1G for weight / scale ratio of 1 and instant death at 5G
        kinetic_damage = log10(kinetic_damage * c1o6) * 0.1;

        //if the damage is indeed above 0 after applying the ratio
        if (kinetic_damage > 0) {
            //apply damage to the core
            core_integrity -= kinetic_damage;

            //reset the shock old shock vector
            shock_damage_new = Ogre::Vector3::ZERO;
            shock_damage_old = Ogre::Vector3::ZERO;
        }

        //if too hard a collision
        if (shock > drive.max_speed * 0.25) {
            //reset the throttle
            controller->setThrottle(0);
        }

        //notify the log of the damage
        if (hud_attached) {
            Game::hud->log->addLine(Game::text->getText(internal_string::shock_damage_sustained)
                                    +" $e"+realIntoString(kinetic_damage * 100, 2)+"$r%");
        }
    }
}

/** checks all the components and assign weight an momentum, done on destruction as well
  * @todo: get radius from the file or bounding box
  */
void Crusader::recalculateWeight()
{
    //TODO: read radius from the bounding box instead
    Ogre::Real radius_squared_over_2 = 2;

    //weights of top half components
    Ogre::Real top_armour_weight = 0;
    {
        using namespace crusader_area;
        top_armour_weight = armour[torso] + armour[torso_right] + armour[torso_left]
                            + armour[arm_right] + armour[arm_left] + armour[torso_back]
                            + armour[torso_right_back] + armour[torso_left_back];
    }
    Ogre::Real top_structure_weight = 0;
    {
        using namespace crusader_part;
        top_structure_weight = structure[torso] + structure[torso_right] + structure[torso_left]
                               + structure[arm_right] + structure[arm_left];
    }

    //weights of all components
    Ogre::Real total_armour_weight = 0;
    {
        using namespace crusader_area;
        total_armour_weight = armour[leg_right] + armour[leg_left] + top_armour_weight;
    }
    Ogre::Real total_structure_weight = 0;
    {
        using namespace crusader_part;
        total_structure_weight = structure[leg_right] + structure[leg_left] + top_structure_weight;
    }

    //total weight
    Ogre::Real total_weight_top = chasis.weight + top_armour_weight
                                  + top_structure_weight + heatsinks;
    total_weight = chasis.weight + drive.weight + engine.weight + total_armour_weight
                   + total_structure_weight + heatsinks;

    //momentums
    angular_momentum = total_weight * radius_squared_over_2;
    angular_momentum_top = total_weight_top * radius_squared_over_2;
}

/** updates all weapons inside the mech
  * @todo: optimise
  */
void Crusader::fireWeapons()
{
    //fire individual groups
    if (take(controller->control_block.fire_group_1))
        fireGroup(0);
    if (take(controller->control_block.fire_group_2))
        fireGroup(1);
    if (take(controller->control_block.fire_group_3))
        fireGroup(2);
    if (take(controller->control_block.fire_group_4))
        fireGroup(3);
    if (take(controller->control_block.fire_group_5))
        fireGroup(4);
    if (take(controller->control_block.fire_group_all)) {
        for (usint i = 0; i < num_of_weapon_groups; ++i) {
            fireGroup(i);
        }
    }

    //fire selected weapon or group
    if (take(controller->control_block.fire)) {
        if (controller->control_block.fire_mode_group) {
            //group fire
            if (fireGroup(current_group)) {
                //if auto cycle enabled cycle group after fire
                if (controller->control_block.auto_cycle) cycleGroup();
            }
        } else {
            //fire individual weapons
            if (current_weapon < design.weapon_groups[current_group].size()) {
                if (weapons[design.weapon_groups[current_group][current_weapon]]->fire()) {
                    //if auto cycle enabled cycle weapon after fire
                    if (controller->control_block.auto_cycle) cycleWeapon();
                }
            }

        }
    }

    //cycling groups manually
    if (take(controller->control_block.cycle_group))
        cycleGroup();

    //cycling weapons manually
    if (take(controller->control_block.cycle_weapon)) {
        //if group fire cycle group instead
        if (controller->control_block.fire_mode_group) {
            cycleGroup();

        } else {
            cycleWeapon();
        }
    }

    //updates all weapons for timeout and firing
    //TODO: make a bitset of active weapons instead of iterating through all maybe?
    for (usint i = 0, for_size = weapons.size(); i < for_size; ++i) {
        weapons[i]->update(dt);
    }
}

/** @brief select next weapon grouop
  */
void Crusader::cycleGroup()
{
    //if it fails to find any operational weapons we mark it so current weapon is -1
    weapons_operational = false;

    //if it fails to find a valid group it will not cycle at all
    for (usint i = 0; i < num_of_weapon_groups; ++i) {
        //wrap around
        if (++current_group == num_of_weapon_groups) current_group = 0;

        //check if group exists
        if (design.weapon_groups[current_group].size() > 0) {
            //so that it starts with the first weapon
            current_weapon = -1;

            //cycle the weapons so it finds the first valid weapon
            cycleWeapon();

            //if the weapon is working we have found our new group
            if (weapons[design.weapon_groups[current_group][current_weapon]]->isOperational()) {
                weapons_operational = true;
                break;
            }
        }
    }
}

/** @brief select next weapon
  */
void Crusader::cycleWeapon()
{
    //if it fails to find any operational weapons we mark it so current weapon is -1
    weapons_operational = false;

    usint weapons_in_group = design.weapon_groups[current_group].size();

    if (weapons_in_group > 0) {
        //if it fails to find a valid weapon it still cycles by one
        for (usint i = 0; i <= weapons_in_group; ++i) {
            //wrap around
            if (++current_weapon == weapons_in_group) current_weapon = 0;

            //if the weapon is working we have found the new weapon
            if (weapons[design.weapon_groups[current_group][current_weapon]]->isOperational()) {
                weapons_operational = true;
                break;
            }
        }
    }
}

/** @brief tries to fire all weapons in group
  * returns true if at least one fires
  */
bool Crusader::fireGroup(usint a_group)
{
    bool fired = false;

    for (usint i = 0, for_size = design.weapon_groups[a_group].size(); i < for_size; ++i) {
        if (weapons[design.weapon_groups[a_group][i]]->fire()) { //fire whole group one by one
            //if at least one fired return true
            fired = true;
        }
    }

    return fired;
}

/** @brief tries to fire all weapons in group
  * returns true if at least one fires
  */
void Crusader::pumpHeat()
{
    Ogre::Real ambient_temperature = Game::arena->getAmbientTemperature(pos_xyz);
    if (controller->control_block.flush_coolant) {
        //if flush coolant key presesed flush for as long as it's pressed
        Ogre::Real amount_flushed = dt; //1 second of flushing depletes 1 unit of coolant

        //change only the coolant level
        coolant_level -= amount_flushed / coolant * 100;

        //if it was the last of the coolant
        if (coolant_level < 0) {
            //cap at zero and ignore effects
            coolant_level = 0;

        } else {
            //otherwise dump some heat
            if (core_temperature > ambient_temperature) {
                //only decrease temp if ambient temp is higher
                core_temperature -= amount_flushed * critical_temperature;
                //1 second of flushing can clear critical temp
            }
        }
    }

    //pump surface to core - mother nature and father science weep
    Ogre::Real difference = armour_conductivity * dt
                            * (core_temperature - surface_temperature) * 0.5;
    surface_temperature += difference;
    core_temperature -= difference;

    //apply heatsinks
    if (core_temperature > ambient_temperature) {
        core_temperature -= dt * heatsinks;
        surface_temperature -= dt * heatsinks; //this is worng but necessary
    }
}

/** @brief move the crusader
  * moving and @todo: slow down on slopes
  */
void Crusader::moveCrusader()
{
    //traction
    Ogre::Real ground_traction = 0.9; //FAKE
    Ogre::Real traction = ground_traction * drive.traction;

    //correct velocity for direction
    corrected_velocity_scalar = velocity.dotProduct(direction);
    Ogre::Vector3 corrected_velocity = corrected_velocity_scalar * direction;
    velocity = (1 - traction) * velocity + traction * corrected_velocity;

    //animate the walking TODO: only take the walking velocity and ignore sliding
    animation->walk(corrected_velocity_scalar);

    //kinemataic resistance
    Ogre::Real ground_resistance = 0.1; //FAKE
    Ogre::Real kinematic_resistance = 0;

    //temp! get the gradient of the slope
    Ogre::Real height = Game::arena->getHeight(pos_xyz.x, pos_xyz.z);
    Ogre::Real height_ahead = Game::arena->getHeight(pos_xyz.x + direction.x,
                                                     pos_xyz.z + direction.z);
    Ogre::Real gradient = height_ahead - height;
    gradient *= gradient;
    if (gradient > 1) gradient = 0;

    //get throttle from controller which is in <-1,1> range
    throttle = controller->getThrottle();

    //difference_of_velocity used to decide whether or not to speed up or slow down
    //takes into account the direction as well, init with current velocity
    Ogre::Real difference_of_velocity = -velocity.dotProduct(direction);

    //now decide depending on direction which values to use
    if (corrected_velocity_scalar > 0) { //use resistance for fwd or reverse depending on velocity
        kinematic_resistance = (drive.kinematic_resistance + ground_resistance) / drive.max_speed;
        difference_of_velocity += throttle * drive.max_speed; //add intended speed
    } else {
        kinematic_resistance = (drive.kinematic_resistance_reverse + ground_resistance)
                               / drive.max_speed_reverse;
        difference_of_velocity += throttle * drive.max_speed_reverse; //add intended speed
    }

    //make the growth square
    kinematic_resistance *= kinematic_resistance;

    //acceleration
    Ogre::Real acceleration_scalar = 0;

    if (throttle < 0.1 && throttle > -0.1) {
        //if no throttle and velocity low just stop immidately
        if (velocity.length() < 0.1) {
            velocity = Ogre::Vector3(0, 0, 0);
        }
    }

    //if difference is smaller then 0.1 don't do antyhing
    if (difference_of_velocity > 0.1) {
        //if the set speed is smaller accelerate
        acceleration_scalar = engine.rating / total_weight;
    } else if (difference_of_velocity < -0.1) {
        //slow down otherwise
        if  (corrected_velocity_scalar > 0) {
            //the crusader is going forward so slow down
            acceleration_scalar = -engine.rating / total_weight;
        } else {
            //crusader is reversing so apply a different rating
            acceleration_scalar = -engine.rating_reverse / total_weight;
        }
    }

    Ogre::Vector3 acceleration = traction * acceleration_scalar * direction;

    //TEMP!!! quick fix for slowing down on slopes
    acceleration -= direction * gradient * corrected_velocity_scalar;

    //new velocity and position
    velocity = velocity - velocity * kinematic_resistance + acceleration * dt;
    move = velocity * dt; //save the difference of positions into move
    pos_xyz = pos_xyz + move;

    //terrain clamping
    pos_xyz.y = crusader_height + Game::arena->getHeight(pos_xyz.x, pos_xyz.z);

    //get where the drive is turning
    Ogre::Radian turning_speed = controller->getTurnSpeed() * -drive.turn_speed;

    //new orientation
    orientation = Ogre::Quaternion((turning_speed * dt), Ogre::Vector3::UNIT_Y) * orientation;
    //update direction vector
    direction = orientation * Ogre::Vector3::UNIT_Z;

    //hook it up to animation
    animation->turn(turning_speed);

    //dust from under the feet
    step_dust->setRate(max(corrected_velocity_scalar, 3 * turning_speed.valueRadians()));
}

/** @brief rotates the torso
  * uses angles in the horzontal plane internally
  * rotates the torso (and possibly arms in the future) @todo: vertical angles and arms?
  */
void Crusader::moveTorso()
{
    //turning the torso
    if(controller->control_block.turn_to_pointer) { //do you want to turn torso
        //direction to target from the mouse pointer
        Ogre::Vector2 target_direction = controller->getPointerPosXZ()
                                         - Ogre::Vector2(pos_xyz.x, pos_xyz.z);
        target_direction.normalise();

        //drive angle in world <-pi,pi>
        Ogre::Vector2 planar_direction(direction.x, direction.z);
        Ogre::Real angle_in_degrees = acos(planar_direction.dotProduct(Ogre::Vector2::UNIT_Y));
        Ogre::Radian drive_angle = Ogre::Radian(angle_in_degrees);
        if (direction.x < 0) {
            drive_angle = -drive_angle;
        }

        //angle to target in world <-pi,pi>
        angle_in_degrees = acos(target_direction.dotProduct(Ogre::Vector2::UNIT_Y));
        Ogre::Radian angle_to_target = Ogre::Radian(angle_in_degrees);
        if (target_direction.x < 0) {
            angle_to_target = -angle_to_target;
        }

        //torso angle in world
        planar_direction = Ogre::Vector2(torso_direction.x, torso_direction.z);
        angle_in_degrees = acos(planar_direction.dotProduct(Ogre::Vector2::UNIT_Y));
        Ogre::Radian torso_angle = Ogre::Radian(angle_in_degrees);
        if (torso_direction.x < 0) {
            torso_angle = -torso_angle;
        }

        //get local angle to target
        localiseAngle(angle_to_target, drive_angle);

        //cap target angle if it exceeds limits of the chasis
        if (angle_to_target > chasis.torso_arc) {
            angle_to_target = chasis.torso_arc;
        } else if (angle_to_target < -chasis.torso_arc) {
            angle_to_target = -chasis.torso_arc;
        }

        //get local angle to torso
        localiseAngle(torso_angle, drive_angle);

        //angle to target from current
        Ogre::Radian angle_to_turn = angle_to_target - torso_angle;

        //get max angle crusader can turn in dt
        Ogre::Radian angle_dt = chasis.torso_turn_speed * dt;

        if (angle_to_turn.valueRadians() < 0) { //check which way to turn and
            angle_dt = -angle_dt;
            if (angle_dt < angle_to_turn) { //if it would overshoot left
                angle_dt = angle_to_turn; //cap to precise angle left
            }
        } else if (angle_dt > angle_to_turn) { //if it would overshoot right
            angle_dt = angle_to_turn; //cap to precise angle right
        }

        //orientation after turn
        torso_orientation = Ogre::Quaternion(angle_dt, Ogre::Vector3::UNIT_Y) * torso_orientation;

        //turn the torso node
        torso_node->setOrientation(torso_orientation);
    }

    //update torso direction even if you're not turning because the drive might be turning
    torso_direction = orientation * torso_orientation * Ogre::Vector3::UNIT_Z;
}

/** @brief local angle based on an angle passed in
  * converts to a <-pi, pi> range angle in relation to global_angle an angle
  */
inline void Crusader::localiseAngle(Ogre::Radian &angle, const Ogre::Radian &global_angle)
{
    Ogre::Radian local_angle;
    Ogre::Radian local_angle_alt;

    if (global_angle < angle) {
        local_angle = angle - global_angle;
        local_angle_alt = Ogre::Radian(2 * pi) - (angle - global_angle);
        if (local_angle > local_angle_alt) {
            local_angle = -local_angle_alt;
        }
    } else {
        local_angle = global_angle - angle;
        local_angle_alt = Ogre::Radian(2 * pi) - (global_angle - angle);
        if (local_angle > local_angle_alt) {
            local_angle = -local_angle_alt;
        }
        local_angle = -local_angle;
    }

    angle = local_angle;
}

/** @brief updateController called by update on every frame
  * physics and controls
  */
int Crusader::updateController()
{
    if (core_integrity > 0) {
        //damage and heat
        shockDamage();
        pumpHeat();

        //weapons and targeting
        fireWeapons();
        updateTargets();

        //moving
        moveCrusader();
        moveTorso();

        //animation loop
        animation->update(dt);
        //radar loop
        radar->update(dt);

        //temp
        if (hud_attached) {
            Game::hud->status->setLine(string("current group$e ")
                                       +intoString(current_group), 1, 20);
            Game::hud->status->setLine(string("current weapon$e ")
                                       +intoString(current_weapon), 1, 20, 21);
            if (controller->control_block.fire_mode_group) {
                Game::hud->status->setLine(string("$egroup$x mode"), 1, 11, 41);
            } else {
                Game::hud->status->setLine(string("$esingle$x mode"), 1, 11, 41);
            }
            if (controller->control_block.auto_cycle) {
                Game::hud->status->setLine(string("$aauto"), 1, 6, 54);
            } else {
                Game::hud->status->setLine(string("$a"), 1, 6, 54);
            }
            Game::hud->status->setLine(string("$rpos x: ")+intoString(pos_xyz.x)+" y: "
                                       +intoString(pos_xyz.z), 0, 20, 0);

            Game::hud->status->setLine(string("integrity: ")+intoString(core_integrity),
                                       0, 20, 21);
        }
    } else if (core_integrity < -1) { //temp!!!
        core_integrity = 0;
        Game::particle_factory->createExplosion(pos_xyz, 10, 2, 3);
        if (hud_attached) {
            Game::hud->log->addLine("your crusader has been destroyed - $eGAME OVER");
        }
    } else {
        core_integrity -= dt;
    }

    if (Game::arena->isOutOfBounds(pos_xyz)) {
        //TODO: either flag this as a bollocked map or if the map has an exit do something here
    }

    return 0; //don't remove crusaders even after death
}
