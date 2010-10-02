//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef COLLISIONTYPE_H
#define COLLISIONTYPE_H

enum collision_type {
    collision_type_blocking,
    collision_type_soft,
    collision_type_hard,
    collision_type_impact
};

//just piggybacking, move along
const usint max_num_cs = 32;
const usint max_num_es = 8;

#endif // COLLISIONTYPE_H
