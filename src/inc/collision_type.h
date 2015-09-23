// (c) Paul Szczepanek (teatimecoder.com). Code released under GPL Version 3.

#ifndef COLLISIONTYPE_H
#define COLLISIONTYPE_H

enum collision_type {
  collision_type_blocking,
  collision_type_soft,
  collision_type_hard,
  collision_type_impact
};

const usint MAX_NUM_CS = 64;
const usint MAX_NUM_ES = 16;

#endif // COLLISIONTYPE_H
