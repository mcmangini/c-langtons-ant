#ifndef ANT_H
#define ANT_H

/*
 * ant.h
 */

#include "point.h"

typedef struct {
    point pos;
    point dir;
    point screen_pos;
} ant;

void center_ant(ant *a, int row, int col, point *grid_offset);
void render_ant(ant *a, int row, int col, point *grid_offset);
void rotate_ant(ant *a, int tile_value, char *pattern);
void set_ant_dir(ant *a, char dir);

#endif
