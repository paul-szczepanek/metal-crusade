// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#include "crusader.h"
#include "game.h"
#include "game_arena.h"
//#include "game_hud.h"
#include "files_handler.h"
#include "corpus_manager.h"
#include "particle_manager.h"
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

Crusader::~Crusader()
{
  for (auto w : Weapons) {
    delete w;
  }

  delete CrusaderAnim;
  //delete radar;
  for (auto p : Parts) {
    delete p;
  }
}

void Crusader::createModel()
{
  // create entities
  Ogre::Entity* drive_mesh;
  Ogre::Entity* chassis_mesh;
  Ogre::Entity* arm_right_mesh;
  Ogre::Entity* arm_left_mesh;
  Ogre::Entity* leg_right_mesh;
  Ogre::Entity* leg_left_mesh;

  drive_mesh = Game::Scene->createEntity(Name + "_drive",
                                         DriveDesign.mesh + "_drive.mesh");
  chassis_mesh = Game::Scene->createEntity(Name + "_chassis",
                                           ChassisDesign.mesh + "_chasis.mesh"); //typo in file
  arm_right_mesh = Game::Scene->createEntity(Name + "_arm_right",
                                             ChassisDesign.mesh + "_arm_right.mesh");
  arm_left_mesh = Game::Scene->createEntity(Name + "_arm_left",
                                            ChassisDesign.mesh + "_arm_left.mesh");
  leg_right_mesh = Game::Scene->createEntity(Name + "_leg_right",
                                             DriveDesign.mesh + "_leg_right.mesh");
  leg_left_mesh = Game::Scene->createEntity(Name + "_leg_left",
                                            DriveDesign.mesh + "_leg_left.mesh");
  // assign materials
  drive_mesh->setMaterialName(CrusaderDesign.material);
  chassis_mesh->setMaterialName(CrusaderDesign.material);
  arm_left_mesh->setMaterialName(CrusaderDesign.material);
  arm_right_mesh->setMaterialName(CrusaderDesign.material);
  leg_left_mesh->setMaterialName(CrusaderDesign.material);
  leg_right_mesh->setMaterialName(CrusaderDesign.material);

  // create root node (drive) and attach chassis node as a child
  Ogre::SceneNode* drive_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  Ogre::SceneNode* torso_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  Ogre::SceneNode* arm_right_node = Game::Scene->getRootSceneNode()->createChildSceneNode();
  Ogre::SceneNode* arm_left_node = Game::Scene->getRootSceneNode()->createChildSceneNode();

  // attach meshes
  drive_node->attachObject(drive_mesh);
  torso_node->attachObject(chassis_mesh);
  arm_right_node->attachObject(arm_right_mesh);
  arm_left_node->attachObject(arm_left_mesh);
  drive_node->attachObject(leg_right_mesh); // legs attached directly to drive
  drive_node->attachObject(leg_left_mesh);

  // set flags to entities
  drive_mesh->setQueryFlags(query_mask_units);
  chassis_mesh->setQueryFlags(query_mask_units);
  arm_left_mesh->setQueryFlags(query_mask_units);
  arm_right_mesh->setQueryFlags(query_mask_units);
  leg_left_mesh->setQueryFlags(query_mask_units);
  leg_right_mesh->setQueryFlags(query_mask_units);

  // center of mech is at the top of the drive so we need the highest mesh attached to the drive
  Ogre::SceneNode::ObjectIterator it = drive_node->getAttachedObjectIterator();
  while (it.hasMoreElements()) {
    Real y = it.getNext()->getBoundingBox().getSize().y;
    if (y > CrusaderHeight) {
      CrusaderHeight = y;
    }
  }

  // get animations
  CrusaderAnim = new Animation();
  // get animation states from all the entities in the crusader
  CrusaderAnim->extractAnimations(drive_node);
  CrusaderAnim->extractAnimations(torso_node);
  CrusaderAnim->extractAnimations(arm_right_node);
  CrusaderAnim->extractAnimations(arm_left_node);

  Parts[crusader_corpus::torso] = new Corpus(this, torso_node);
  Parts[crusader_corpus::drive] = new Corpus(this, drive_node);
  Parts[crusader_corpus::arm_r] = new Corpus(this, arm_right_node);
  Parts[crusader_corpus::arm_l] = new Corpus(this, arm_left_node);

  for (Corpus* p : Parts) {
    Game::Corpus->registerDynamicObject(p);
  }

  Parts[crusader_corpus::torso]->loadCollision(ChassisDesign.mesh);
  Parts[crusader_corpus::drive]->loadCollision(ChassisDesign.mesh + "_drive");

  // assign dust emmitter for walking on surfaces producing particles
  Ogre::SceneNode* step_dust_node = drive_node->createChildSceneNode();
  // put the scene node at ground level
  step_dust_node->setPosition(Vector3(0, -CrusaderHeight, 0));
  StepDust = static_cast<ParticleEffectStepDust*>(Game::Particle->createStepDust(step_dust_node));
}

/** This reads the Crusader definition from a file and creates one
 * @todo: add more internal types and read things from the mesh
 */
Crusader::Crusader(const string&      a_unit_name,
                   Vector3            a_pos_xyz,
                   Quaternion         a_orientation,
                   crusader_design_t  a_design,
                   crusader_engine_t  a_engine,
                   crusader_drive_t   a_drive,
                   crusader_chassis_t a_chassis)
  : Unit(a_unit_name),
  CrusaderDesign(a_design),
  EngineDesign(a_engine),
  DriveDesign(a_drive),
  ChassisDesign(a_chassis)
{
  CoreIntegrity = 1.0;

  // resize vectors to fit the num of parts and areas
  StructureIntegrity.resize(ChassisDesign.num_of_parts, 0);
  ArmourIntegrity.resize(ChassisDesign.num_of_areas, 0);

  createModel();
  // get positions of panels and translate to weapon positions
  positionWeapons();

  // add heatsinks
  Heatsinks = count(CrusaderDesign.internals.begin(),
                    CrusaderDesign.internals.end(), internal_type_heatsink);

  // add internal_strucuture elements to base structure
  for (size_t i = 0; i < ChassisDesign.num_of_parts; ++i) {
    StructureIntegrity[i] = ChassisDesign.structure_base;

    // go through all internals
    for (usint j = 0, for_size = CrusaderDesign.internals.size(); j < for_size; ++j) {
      // check if it's a structure and in the right body part
      if (CrusaderDesign.internals[j] == internal_type_structure
          && CrusaderDesign.internals_placement[j] == i) {
        // add it to the strucutre of that body part
        StructureIntegrity[i] += 1;
      }
    }
  }

  // assign armour from design
  for (size_t i = 0; i < ChassisDesign.num_of_areas; ++i) {
    assert(ChassisDesign.surface_area[i] > 0);
    ArmourIntegrity[i] = CrusaderDesign.armour_placement[i]; // temp? / chassis.surface_area[i];
  }

  // get armour characteristics
  ArmourStructure = armour_type_to_structure[static_cast<size_t>(CrusaderDesign.armour)];
  ArmourBallistic = armour_type_to_ballistic[static_cast<size_t>(CrusaderDesign.armour)];
  ArmourConductivity = armour_type_to_conductivity[static_cast<size_t>(CrusaderDesign.armour)];
  ArmourGeneratedHeat = armour_type_to_generated_heat[static_cast<size_t>(CrusaderDesign.armour)];

  // TODO: load equipment
  // design.equipment;

  // TODO: load coolant from design
  CoolantQuantity = 4; // temp

  // after loading all the weapons etc. recalculate weight
  recalculateWeight();

  // terrain clamping
  a_pos_xyz.y = CrusaderHeight + Game::Arena->getHeight(a_pos_xyz.x, a_pos_xyz.z);
  for (Corpus* p : Parts) {
    p->XYZ = a_pos_xyz;
    p->Weight = TotalWeight;
    p->Friction = 0.9;
    p->Hardness = 0.8;
    p->SurfaceTemperature = CoreTemperature;
    p->CollisionType = collision_type_soft;
  }

  // at start align torso with drive
  setOrientation(a_orientation);
  setDriveOrientation(a_orientation);

  // set temperature to ambient temp
  EngineTemperature = CoreTemperature = Game::Arena->getAmbientTemperature(a_pos_xyz);

  // create the radar
  Radar = new RadarComputer(CrusaderDesign.radar, this);
}

/** @brief resolves collision including damage and physics
 */
bool Crusader::handleCollision(Collision* a_collision)
{
  // figure out which colliding object belongs to us
  const bool first = (a_collision->Object[0]->OwnerEntity == (ArenaEntity*)this);
  const size_t ci1 = first ? 0 : 1;
  const size_t ci2 = first ? 1 : 0;
  // move this to a separate function
  Corpus* hit = a_collision->Object[ci2];
  Corpus* corpus = a_collision->Object[ci1];

  if (a_collision->ResultType == collision_type_impact) {
    hit->CollisionType = collision_type_hard;
    // get affected body parts
    const vector<usint> body_areas_hit = a_collision->getAreasIndexes(ci1);

    // spread the damage evenly among hit parts this is mostly for splash damage
    Real damage_spread = 1.0 / body_areas_hit.size();

    // damage
    Real ballistic_dmg = hit->getBallisticDmg() * damage_spread;
    Real energy_dmg = hit->getEnergyDmg() * damage_spread;
    Real heat_dmg = hit->getHeatDmg() * damage_spread;

    // summed for all the parts
    Real heat = 0;

    for (size_t i = 0, for_size = body_areas_hit.size(); i < for_size; ++i) {
      // armour with penetration applied
      const size_t bi = body_areas_hit[i];
      Real effective_armour = ArmourIntegrity[bi] / (hit->getPenetration() + 1);

      // ballistic damage
      Real damage = ballistic_dmg / (effective_armour * ArmourBallistic + 1);

      // energy damage
      damage += energy_dmg / (effective_armour * ArmourConductivity + 1);

      // heat from damage
      Real local_armour_conductivity = ArmourConductivity; // / (effective_armour + 1);
      heat += heat_dmg * local_armour_conductivity;

      // heat generated from armour reaction
      heat += (ballistic_dmg + energy_dmg) * ArmourGeneratedHeat;

      // armour damage proportional to coverage
      Real armour_damage = damage * (ArmourIntegrity[bi]
                                     / CrusaderDesign.armour_placement[bi]);
      ArmourIntegrity[bi] -= (armour_damage / ArmourStructure);
      // cap armour at 0
      if (ArmourIntegrity[bi] < 0) {
        ArmourIntegrity[bi] = 0;
      }

      // all the rest goes to structure
      damage -= armour_damage;

      // if damage was greater than what armour could stop
      if (damage > 0) {
        // translate body area into body part
        crusader_area::body_area area_hit = (crusader_area::body_area)bi;
        crusader_part::body_part part_hit;

        // areas at the back translated to the same part as the front
        if (area_hit == crusader_area::torso_back) {
          part_hit = crusader_part::torso;

        } else if (area_hit == crusader_area::torso_left_back) {
          part_hit = crusader_part::torso_left;

        } else if (area_hit == crusader_area::torso_right_back) {
          part_hit = crusader_part::torso_right;

        } else {
          part_hit = static_cast<crusader_part::body_part>(area_hit);
        }

        // damage the part's structure
        StructureIntegrity[part_hit] -= damage;

        // if goes below 0 move the damage to integrity // TODO: destroying limbs
        if (StructureIntegrity[part_hit] < 0) {
          // subtract negative structure from core integrity
          CoreIntegrity += StructureIntegrity[part_hit] / 10; //temp
          StructureIntegrity[part_hit] = 0;
        }
      }
    }
  }

  // apply heat damage (weight used for scaling as above)
  //SurfaceTemperature += heat / chassis.weight;

  // they are all coupled and share the weight
  // this is incorrect but let's see if I can't get away with it
  setVelocity(corpus->Velocity);

  return true;
}

#define MAX_GRADIENT ((Real)1)

/** @brief move the crusader
 * moving and @todo: slow down on slopes
 */
void Crusader::moveCrusader(const Real a_dt)
{
  // velocity of actual travel, might not even be the intended direction
  const Vector3& velocity = getVelocity();
  // get throttle from Controller which is in <-1,1> range
  Throttle = Controller->getThrottle();

  // find InputVelocity which is the current velocity along the intended direction of travel
  if (Throttle < 0.1 && Throttle > -0.1 && velocity.length() < 0.1) {
    // if no throttle and velocity low just stop immediately
    InputVelocity = Vector3(0, 0, 0);
  } else {
    // drive adapts to the terrain and adds velocity parallel to the ground
    const Vector3& xyz = getXYZ();
    const Real height = Game::Arena->getHeight(xyz.x, xyz.z);
    const Vector3& direction = getDriveDirection();
    const Real x2 = xyz.x + direction.x;
    const Real z2 = xyz.z + direction.z;
    const Real height_ahead = Game::Arena->getHeight(x2, z2);
    // find the slope gradient and limit it
    Real gradient = height_ahead - height;
    const Real sign = (gradient < 0) ? -1 : 1;
    //TODO: max gradient should be part of drive design
    if (sign * gradient > MAX_GRADIENT) {
      gradient = sign * MAX_GRADIENT;
    }

    // drive direction along the terrain within a limited gradient
    Vector3 along_terrain(x2 - xyz.x, gradient, z2 - xyz.z);
    along_terrain.normalise();

    // current velocity along the direction we want to continue travelling
    CorrectedVelocityScalar = velocity.dotProduct(along_terrain);
    InputVelocity = CorrectedVelocityScalar * along_terrain;

    // compare current speed with what the pilot set based on throttle and drive design

    // difference_of_velocity used to decide whether or not to speed up or slow down
    // init with -current velocity (same as subtracting it later)
    Real difference_of_velocity = -CorrectedVelocityScalar;
    if (Throttle < 0) {
      difference_of_velocity += Throttle * DriveDesign.max_speed_reverse;
    } else {
      difference_of_velocity += Throttle * DriveDesign.max_speed;
    }

    // find acceleration added by the drive based on the found speed difference
    // TODO: avoid overshoot

    Real acceleration = 0;

    // if difference is smaller then 0.1 don't do anything
    if (difference_of_velocity > 0.1) {
      // if the set speed is smaller accelerate
      acceleration = EngineDesign.rating / TotalWeight;
    } else if (difference_of_velocity < -0.1) {
      // slow down otherwise
      if  (CorrectedVelocityScalar > 0) {
        // the crusader is going forward so slow down
        acceleration = -EngineDesign.rating / TotalWeight;
      } else {
        // crusader is reversing so apply a different rating
        acceleration = -EngineDesign.rating_reverse / TotalWeight;
      }
    }

    // change the InputVelocity by resistance and acceleration added by the drive

    // traction
    const Real ground_traction = 0.9; // FAKE
    const Real traction = ground_traction * DriveDesign.traction;
    // kinematic resistance
    const Real kinematic_resistance = (CorrectedVelocityScalar < 0) ?
                                      DriveDesign.kinematic_resistance_reverse
                                      : DriveDesign.kinematic_resistance;

    acceleration *= traction * a_dt * a_dt;
    acceleration -= CorrectedVelocityScalar * kinematic_resistance * a_dt;
    InputVelocity = acceleration * along_terrain;
  }

  setVelocity(velocity + InputVelocity);

  // get where the drive is turning
  Radian turning_speed = Controller->getTurnSpeed() * -DriveDesign.turn_speed;

  // new Orientation
  Quaternion orientation_change((turning_speed * a_dt), Vector3::UNIT_Y);
  setDriveOrientation(orientation_change * getDriveOrientation());
  setOrientation(orientation_change * getOrientation());

  // hook it up to animation
  CrusaderAnim->turn(turning_speed);
  CrusaderAnim->walk(CorrectedVelocityScalar);

  // dust from under the feet
  StepDust->setRate(max(CorrectedVelocityScalar, 3 * turning_speed.valueRadians()));
}

/** @brief rotates the torso
 * uses angles in the horizontal plane internally
 * rotates the torso (and possibly arms in the future) @todo: vertical angles and arms?
 */
void Crusader::moveTorso(const Real a_dt)
{
  // turning the torso
  if(Controller->ControlBlock.turn_to_pointer) { // do you want to turn torso
    // Direction to target from the mouse pointer
    Vector2 target_direction = Controller->getPointerPosXZ()
                               - Vector2(getXYZ().x, getXYZ().z);
    target_direction.normalise();

    // drive angle in world <-pi,pi>
    const Vector3& drive_direction = getDriveDirection();
    Vector2 planar_direction(drive_direction.x, drive_direction.z);
    Real angle_in_degrees = acos(planar_direction.dotProduct(Vector2::UNIT_Y));
    Radian drive_angle = Radian(angle_in_degrees);
    if (drive_direction.x < 0) {
      drive_angle = -drive_angle;
    }

    // angle to target in world <-pi,pi>
    angle_in_degrees = acos(target_direction.dotProduct(Vector2::UNIT_Y));
    Radian angle_to_target = Radian(angle_in_degrees);
    if (target_direction.x < 0) {
      angle_to_target = -angle_to_target;
    }

    // torso angle in world
    const Vector3& direction = getDirection();
    planar_direction = Vector2(direction.x, direction.z);
    angle_in_degrees = acos(planar_direction.dotProduct(Vector2::UNIT_Y));
    Radian torso_angle = Radian(angle_in_degrees);
    if (direction.x < 0) {
      torso_angle = -torso_angle;
    }

    // get local angle to target
    localiseAngle(angle_to_target, drive_angle);

    // cap target angle if it exceeds limits of the chassis
    if (angle_to_target > ChassisDesign.torso_arc) {
      angle_to_target = ChassisDesign.torso_arc;
    } else if (angle_to_target < -ChassisDesign.torso_arc) {
      angle_to_target = -ChassisDesign.torso_arc;
    }

    // get local angle to torso
    localiseAngle(torso_angle, drive_angle);

    // angle to target from current
    Radian angle_to_turn = angle_to_target - torso_angle;

    // get max angle crusader can turn in a_dt
    Radian angle_dt = ChassisDesign.torso_turn_speed * a_dt;

    if (angle_to_turn.valueRadians() < 0) { // check which way to turn and
      angle_dt = -angle_dt;
      if (angle_dt < angle_to_turn) { // if it would overshoot left
        angle_dt = angle_to_turn; // cap to precise angle left
      }
    } else if (angle_dt > angle_to_turn) { // if it would overshoot right
      angle_dt = angle_to_turn; // cap to precise angle right
    }

    // Orientation after turn
    Quaternion orientation_change(angle_dt, Vector3::UNIT_Y);
    setOrientation(orientation_change * getOrientation());
  }

  setXYZ(Parts[crusader_corpus::torso]->XYZ);
}

/** damage suffered from G forces
 */
void Crusader::shockDamage(const Real a_dt)
{
  // to make sure a_dt is sane (1 - span) is > 0
  // Real span = min(a_dt, Real(0.1)); // already guaranteed by timer

  // average over span
  ShockDamageNew = ShockDamageNew * (1 - a_dt * 10) + InputVelocity * a_dt * 10;
  // average over a two spans
  ShockDamageOld = ShockDamageOld * (1 - a_dt) + InputVelocity * a_dt;

  // get the change in velocity
  Real shock = (ShockDamageNew - ShockDamageOld).length();

  // ignore 0.6G force changes as they would likely be tiny anyway
  if (shock > 6) {
    // damage multipilied by weight / scale ratio
    Real kinetic_damage = shock * (TotalWeight / ChassisDesign.weight);

    // with zero damage at 1G for weight / scale ratio of 1 and instant death at 5G
    kinetic_damage = log10(kinetic_damage * c1o6) * 0.1;

    // if the damage is indeed above 0 after applying the ratio
    if (kinetic_damage > 0) {
      // apply damage to the core
      CoreIntegrity -= kinetic_damage;

      // reset the shock old shock vector
      ShockDamageNew = Vector3::ZERO;
      ShockDamageOld = Vector3::ZERO;
    }

    // if too hard a collision
    if (shock > DriveDesign.max_speed * 0.25) {
      // reset the throttle
      Controller->setThrottle(0);
    }

    // notify the log of the damage
    if (HudAttached) {
       Game::Hud->log->addLine(Game::Text->getText(internal_string::shock_damage_sustained)
       + " $e" + realIntoString(kinetic_damage * 100, 2) + "$r%");
    }
  }
}

/** checks all the components and assign weight an momentum, done on destruction as well
 * @todo: get radius from the file or bounding box
 */
void Crusader::recalculateWeight()
{
  // TODO: read radius from the bounding box instead
  Real radius_squared_over_2 = 2;

  // weights of top half components
  Real top_armour_weight = 0;
  {
    using namespace crusader_area;
    top_armour_weight = ArmourIntegrity[torso] + ArmourIntegrity[torso_right] +
                        ArmourIntegrity[torso_left]
                        + ArmourIntegrity[arm_right] + ArmourIntegrity[arm_left] +
                        ArmourIntegrity[torso_back]
                        + ArmourIntegrity[torso_right_back] + ArmourIntegrity[torso_left_back];
  }
  Real top_structure_weight = 0;
  {
  using namespace crusader_part;
  top_structure_weight = StructureIntegrity[torso] + StructureIntegrity[torso_right] +
                         StructureIntegrity[torso_left]
                         + StructureIntegrity[arm_right] + StructureIntegrity[arm_left];
  }

  // weights of all components
  Real total_armour_weight = 0;
  {
  using namespace crusader_area;
  total_armour_weight = ArmourIntegrity[leg_right] + ArmourIntegrity[leg_left] + top_armour_weight;
  }
  Real total_structure_weight = 0;
  {
  using namespace crusader_part;
  total_structure_weight = StructureIntegrity[leg_right] + StructureIntegrity[leg_left] +
                           top_structure_weight;
  }

  // total weight
  Real total_weight_top = ChassisDesign.weight + top_armour_weight
                          + top_structure_weight + Heatsinks;
  TotalWeight = ChassisDesign.weight + DriveDesign.weight + EngineDesign.weight +
                total_armour_weight
                + total_structure_weight + Heatsinks;

  // momentums
  AngularMomentum = TotalWieght * radius_squared_over_2;
  AngularMomentumTop = total_weight_top * radius_squared_over_2;
}

/** gets positions for all the panels used to mount weapons
 * // TEMP!!!
 * @todo: read from mesh instead
 */
void Crusader::positionWeapons()
{
  // positioning weapons
  size_t total_panels = 0; // count panels
  for (size_t i = 0; i < ChassisDesign.num_of_parts; ++i) {
    total_panels += ChassisDesign.panels[i];
  }
  vector<usint> slots_used(ChassisDesign.num_of_parts, 0);
  vector<Vector3> panel_positions(total_panels);

  // get positions of panels and tranlate to weapon positions
  // fake - position should be read from mesh
  vector<Vector3> positions(ChassisDesign.num_of_parts, Vector3(0, 0, 0));
  positions[0] = (Vector3(0, -0.9, 3));
  positions[1] = (Vector3(2, -0.9, 3));
  positions[2] = (Vector3(-2, -0.9, 3));
  positions[3] = (Vector3(3.8, -0.9, 3));
  positions[4] = (Vector3(-3.8, -0.9, 3));
  positions[5] = (Vector3(3.8, -0.9, 3));
  positions[6] = (Vector3(-3.8, -0.9, 3));
  // positions[7] = (Vector3(3.8, -1.9, 3)); // legs unused for now at least
  // positions[8] = (Vector3(-3.8, -1.9, 3));

  // translate panel positions to weapon positions
  usint k = 0;
  for (usint j = 0; j < ChassisDesign.num_of_parts; ++j) {
    slots_used[j] = k;
    for (size_t i = 0; i < ChassisDesign.panels[j]; ++i) {
      panel_positions[k] = positions[j];
      ++k;
    }
  }

  // create weapons at those mount points
  for (size_t i = 0, for_size = CrusaderDesign.weapons.size(); i < for_size; ++i) {
    Weapons.push_back(new Weapon(CrusaderDesign.weapons[i], this,
                                 panel_positions[slots_used[CrusaderDesign.weapons_placement[i]]],
                                 // if extra ammo vector exists at this point
                                 (i < CrusaderDesign.weapons_extra_ammo.size()) ?
                                 CrusaderDesign.weapons_extra_ammo[i] // use it
                                 : 0)); // oterwise assume no extra ammo

    // also adds extra ammo if available for that weapon
    slots_used[CrusaderDesign.weapons_placement[i]] += Weapons[i]->weapon_design.panels;
  }
}

/** updates all weapons inside the mech
 * @todo: optimise
 */
void Crusader::updateWeapons(const Real a_dt)
{
  // fire individual groups
  if (take(Controller->ControlBlock.fire_group_1)) {
    fireGroup(0);
  }
  if (take(Controller->ControlBlock.fire_group_2)) {
    fireGroup(1);
  }
  if (take(Controller->ControlBlock.fire_group_3)) {
    fireGroup(2);
  }
  if (take(Controller->ControlBlock.fire_group_4)) {
    fireGroup(3);
  }
  if (take(Controller->ControlBlock.fire_group_5)) {
    fireGroup(4);
  }
  if (take(Controller->ControlBlock.fire_group_all)) {
    for (size_t i = 0; i < NUM_OF_WEAPON_GROUPS; ++i) {
      fireGroup(i);
    }
  }

  // fire selected weapon or group
  if (take(Controller->ControlBlock.fire)) {
    if (Controller->ControlBlock.fire_mode_group) {
      // group fire
      if (fireGroup(CurrentGroup)) {
        // if auto cycle enabled cycle group after fire
        if (Controller->ControlBlock.auto_cycle) { cycleGroup(); }
      }
    } else {
      // fire individual weapons
      if (CurrentWeapon < CrusaderDesign.weapon_groups[CurrentGroup].size()) {
        if (Weapons[CrusaderDesign.weapon_groups[CurrentGroup][CurrentWeapon]]->fire()) {
          // if auto cycle enabled cycle weapon after fire
          if (Controller->ControlBlock.auto_cycle) {
            cycleWeapon();
          }
        }
      }
    }
  }

  // cycling groups manually
  if (take(Controller->ControlBlock.cycle_group)) {
    cycleGroup();
  }

  // cycling weapons manually
  if (take(Controller->ControlBlock.cycle_weapon)) {
    // if group fire cycle group instead
    if (Controller->ControlBlock.fire_mode_group) {
      cycleGroup();

    } else {
      cycleWeapon();
    }
  }

  // updates all weapons for timeout and firing
  for (auto w : Weapons) {
    w->update(a_dt);
  }
}

/** @brief select next weapon grouop
 */
void Crusader::cycleGroup()
{
  // if it fails to find any operational weapons we mark it so current weapon is -1
  WeaponsOperational = false;

  // if it fails to find a valid group it will not cycle at all
  for (size_t i = 0; i < NUM_OF_WEAPON_GROUPS; ++i) {
    // wrap around
    if (++CurrentGroup == NUM_OF_WEAPON_GROUPS) { CurrentGroup = 0; }

    // check if group exists
    if (CrusaderDesign.weapon_groups[CurrentGroup].size() > 0) {
      // so that it starts with the first weapon
      CurrentWeapon = -1;

      // cycle the weapons so it finds the first valid weapon
      cycleWeapon();

      // if the weapon is working we have found our new group
      if (Weapons[CrusaderDesign.weapon_groups[CurrentGroup][CurrentWeapon]]->isOperational()) {
        WeaponsOperational = true;
        break;
      }
    }
  }
}

/** @brief select next weapon
 */
void Crusader::cycleWeapon()
{
  // if it fails to find any operational weapons we mark it so current weapon is -1
  WeaponsOperational = false;

  usint weapons_in_group = CrusaderDesign.weapon_groups[CurrentGroup].size();

  if (weapons_in_group > 0) {
    // if it fails to find a valid weapon it still cycles by one
    for (size_t i = 0; i <= weapons_in_group; ++i) {
      // wrap around
      if (++CurrentWeapon == weapons_in_group) { CurrentWeapon = 0; }

      // if the weapon is working we have found the new weapon
      if (Weapons[CrusaderDesign.weapon_groups[CurrentGroup][CurrentWeapon]]->isOperational()) {
        WeaponsOperational = true;
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

  for (size_t i = 0, for_size = CrusaderDesign.weapon_groups[a_group].size(); i < for_size; ++i) {
    if (Weapons[CrusaderDesign.weapon_groups[a_group][i]]->fire()) { // fire whole group one by one
      // if at least one fired return true
      fired = true;
    }
  }

  return fired;
}

void Crusader::pumpHeat(const Real a_dt)
{
  Real ambient_temperature = Game::Arena->getAmbientTemperature(getXYZ());
  if (Controller->ControlBlock.flush_coolant) {
    // if flush coolant key presesed flush for as long as it's pressed
    Real amount_flushed = a_dt; // 1 second of flushing depletes 1 unit of coolant

    // change only the coolant level
    CoolantLevel -= amount_flushed / CoolantQuantity * 100;

    // if it was the last of the coolant
    if (CoolantLevel < 0) {
      // cap at zero and ignore effects
      CoolantLevel = 0;

    } else {
      // otherwise dump some heat
      if (CoreTemperature > ambient_temperature) {
        // only decrease temp if ambient temp is higher
        CoreTemperature -= amount_flushed * CRITICAL_TEMPERATURE;
        // 1 second of flushing can clear critical temp
      }
    }
  }

  // pump surface to core
  for (auto p : Parts) {
    Real difference = ArmourConductivity * a_dt * (CoreTemperature - p->SurfaceTemperature) * 0.5;
    CoreTemperature -= difference;
    p->SurfaceTemperature += difference;
  }

  // apply heatsinks
  if (CoreTemperature > ambient_temperature) {
    CoreTemperature -= a_dt * Heatsinks;
  }
}

/** @brief updateController called by update on every frame
 * physics and controls
 */
bool Crusader::update(const Real a_dt)
{
  if (CoreIntegrity > 0) {
    shockDamage(a_dt);
    pumpHeat(a_dt);

    updateWeapons(a_dt);
    updateTargets();

    moveCrusader(a_dt);
    moveTorso(a_dt);

    CrusaderAnim->update(a_dt);
    //radar->update(a_dt);

    // temp
    if (HudAttached) {
       Game::Hud->status->setLine(string("current group$e ")
     + intoString(CurrentGroup), 1, 20);
       Game::Hud->status->setLine(string("current weapon$e ")
     + intoString(CurrentWeapon), 1, 20, 21);
       if (Controller->ControlBlock.fire_mode_group) {
        Game::Hud->status->setLine(string("$egroup$x mode"), 1, 11, 41);
       } else {
        Game::Hud->status->setLine(string("$esingle$x mode"), 1, 11, 41);
       }
       if (Controller->ControlBlock.auto_cycle) {
        Game::Hud->status->setLine(string("$aauto"), 1, 6, 54);
       } else {
        Game::Hud->status->setLine(string("$a"), 1, 6, 54);
       }
       Game::Hud->status->setLine(string("$rpos x: ") + intoString(getXYZ().x) + " y: "
     + intoString(getXYZ().z), 0, 20, 0);

       Game::Hud->status->setLine(string("integrity: ") + intoString(CoreIntegrity),
                                 0, 20, 21);
     }
  } else if (CoreIntegrity < -1) { // temp!!!
    CoreIntegrity = 0;
    Game::Particle->createExplosion(getXYZ(), 10, 2, 3);
    if (HudAttached) {
       Game::Hud->log->addLine("your crusader has been destroyed - $eGAME OVER");
    }
  } else {
    CoreIntegrity -= a_dt;
  }

  return true;
}

Vector3 Crusader::getDirection()
{
  return Parts[crusader_corpus::torso]->Direction;
}

Vector3 Crusader::getDriveDirection()
{
  return Parts[crusader_corpus::drive]->Direction;
}

Quaternion Crusader::getOrientation()
{
  return Parts[crusader_corpus::torso]->Orientation;
}

Quaternion Crusader::getDriveOrientation()
{
  return Parts[crusader_corpus::drive]->Orientation;
}

void Crusader::setOrientation(Quaternion a_orientation)
{
  const Vector3 direction = a_orientation * Vector3(0, 0, 1);
  Parts[crusader_corpus::torso]->Orientation = a_orientation;
  Parts[crusader_corpus::torso]->Direction = direction;
  Parts[crusader_corpus::arm_l]->Orientation = a_orientation;
  Parts[crusader_corpus::arm_l]->Direction = direction;
  Parts[crusader_corpus::arm_r]->Orientation = a_orientation;
  Parts[crusader_corpus::arm_r]->Direction = direction;
}

void Crusader::setDriveOrientation(Quaternion a_orientation)
{
  Parts[crusader_corpus::drive]->Orientation = a_orientation;
  Parts[crusader_corpus::drive]->Direction = a_orientation * Vector3(0, 0, 1);
}

const Vector3& Crusader::getXYZ()
{
  return Parts[crusader_corpus::drive]->XYZ;
}

const Vector3& Crusader::getVelocity()
{
  return Parts[crusader_corpus::drive]->Velocity;
}

void Crusader::setXYZ(const Vector3& a_pos)
{
  Parts[crusader_corpus::torso]->XYZ = a_pos;
  Parts[crusader_corpus::drive]->XYZ = a_pos;
  Parts[crusader_corpus::arm_r]->XYZ = a_pos;
  Parts[crusader_corpus::arm_l]->XYZ = a_pos;
}

void Crusader::setVelocity(const Vector3& a_velocity)
{
  Parts[crusader_corpus::torso]->Velocity = a_velocity;
  Parts[crusader_corpus::drive]->Velocity = a_velocity;
  Parts[crusader_corpus::arm_r]->Velocity = a_velocity;
  Parts[crusader_corpus::arm_l]->Velocity = a_velocity;
}
