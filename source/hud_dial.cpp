//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#include "hud_dial.h"
#include "game.h"
#include "hud.h"
#include "unit.h"

HudDial::HudDial(hud_part_design_t& a_hud_part_design) : HudPart(a_hud_part_design)
{
    //read params from design
    if (a_hud_part_design.parameters.size() < 4) //kill the game if too few params
        Game::kill(string("hud_part missing params: ")+a_hud_part_design.name);
    min_value = a_hud_part_design.parameters[0];
    max_value = a_hud_part_design.parameters[1];
    min_angle = a_hud_part_design.parameters[2];
    max_angle = a_hud_part_design.parameters[3];

    //start with min_value
    value = min_value;

    //create texture for the dial
    Ogre::MaterialPtr material = Hud::createOverlayMaterial(a_hud_part_design.name);
    dial_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);

    //apply it to the main container
    container->setMaterialName(a_hud_part_design.name);
    //repostition to centre around x, y
    container->setPosition(position.first - (size.first * 0.5),
                           position.second - (size.second * 0.5));

    //bind the appropriate function for getting the value to the funciton pointer
    if (function == hud_part_enum::speed) {
        getValue = getValueSpeed;
    } else if (function == hud_part_enum::pressure) {
        getValue = getValuePressure;
    } else if (function == hud_part_enum::coolant) {
        getValue = getValueCoolant;
    } else if (function == hud_part_enum::temperature_engine) {
        getValue = getValueEngineTemperature;
    } else if (function == hud_part_enum::temperature) {
        getValue = getValueCoreTemperature;
    } else if (function == hud_part_enum::temperature_external) {
        getValue = getValueExternalTemperature;
    } else if (function == hud_part_enum::throttle) {
        getValue = getValueThrottle;
    } else { //not handled or not handled yet
        getValue = getValueZero;
    }
}

void HudDial::update(Ogre::Real a_dt)
{
    //get approriate value dependiong on dial function
    Ogre::Real new_value = getValue();
    //smooth angle change //doesn't use dt to save on checking
    value = value * 0.9 + new_value * 0.1;

    //translate value to angle
    angle = min_angle + (max_angle - min_angle) * ((value - min_value) / (max_value - min_value));
    angle = min_angle + (max_angle - min_angle) * ((value - min_value) / (max_value - min_value));

    //rotate dial by rotating the texture UVs
    dial_texture->setTextureRotate(-1 * Ogre::Radian(angle * (pi / 180)));
}

//functions to be used to get the value for the dial
//bound to at creation to save on constant checking every frame
Ogre::Real HudDial::getValueSpeed()
{
    return Game::hud->player_unit->getSpeed() * 3.6; //convert to km/h
}

Ogre::Real HudDial::getValuePressure()
{
    return Game::hud->player_unit->getPressure() * 100;
}

Ogre::Real HudDial::getValueCoolant()
{
    return Game::hud->player_unit->getCoolant();
}

Ogre::Real HudDial::getValueEngineTemperature()
{
    return Game::hud->player_unit->getEngineTemperature();
}

Ogre::Real HudDial::getValueCoreTemperature()
{
    return Game::hud->player_unit->getCoreTemperature();
}

Ogre::Real HudDial::getValueExternalTemperature()
{
    return Game::hud->player_unit->getSurfaceTemperature();
}

Ogre::Real HudDial::getValueThrottle()
{
    return Game::hud->player_unit->getThrottle() * 3.6; //convert to km/h
}

Ogre::Real HudDial::getValueZero()
{
    return 0;
}
