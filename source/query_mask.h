//Copyright 2010 Paul Szczepanek. Code released under GPL Version 3.

#ifndef QUERYMASK_H_INCLUDED
#define QUERYMASK_H_INCLUDED

enum query_mask {
    query_mask_pointer_floor = 1 << 0,
    query_mask_units = 1 << 1,
    query_mask_projectiles = 1 << 2,
    query_mask_ignore = 1 << 3,
    query_mask_scenery = 1 << 4
};

#endif // QUERYMASK_H_INCLUDED
