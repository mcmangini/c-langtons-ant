#ifndef GRID_H
#define GRID_H

/*
 * grid.h
 */

#include "ant.h"
#include "point.h"

#define GRID_SIZE_Y 201
#define GRID_SIZE_X 201
#define MAX_COLORS 16

enum offset_direction {
    LEFT,
    UP,
    RIGHT,
    DOWN,
    ZERO
};

enum tile_color {
    BLACK,
    WHITE,
    BLUE,
    GREEN,
    MAGENTA,
    YELLOW,
    CYAN,
    RED
};

void change_tile(int grid[][GRID_SIZE_X], point *p, int pattern_len);
void init_grid(int grid[][GRID_SIZE_X]);
void render_grid(int grid[][GRID_SIZE_X], int row, int col,
        point *grid_offset);
void update_grid(int grid[][GRID_SIZE_X], ant *a, char *pattern);
void update_offset(point *grid_offset, enum offset_direction dir);

#endif
