/*
 * ant.c
 */

#include <ctype.h>
#include <ncurses.h>
#include <string.h>

#include "ant.h"
#include "grid.h"

void center_ant(ant *a, int row, int col, point *grid_offset)
{
    /*
     * Modify the grid offset so that ant *a is centered in the terminal.
     *
     * param a - Pointer to the ant to center
     * param row - The number of rows (lines) displayed by the terminal
     * param col - The number of columns displayed by the terminal
     * param grid_offset - Pointer to the grid offset to be modified
     */
    point center_p;

    center_p.y = row / 2;
    center_p.x = col / 2;
    grid_offset->y += center_p.y - a->screen_pos.y;
    grid_offset->x += center_p.x - a->screen_pos.x;
}

void render_ant(ant *a, int row, int col, point *grid_offset)
{
    /*
     * Draw the ant in the terminal.
     *
     * param a - Pointer to the ant to draw
     * param row - The number of rows (lines) displayed by the terminal
     * param col - The number of columns displayed by the terminal
     * param grid_offset - Pointer to the grid offset
     */
    int acs_dir_sym = 0;
    point start_p = {(row - GRID_SIZE_Y) / 2, (col - GRID_SIZE_X * 2) / 2};
    int y = start_p.y + grid_offset->y + a->pos.y;
    int x = start_p.x + grid_offset->x + a->pos.x * 2;

    // Make sure we use even columns
    if (x % 2 != 0) {
        x += 1;
    }
    // Set a different symbol for the ant depending on its direction
    if (a->dir.y == 0 && a->dir.x == -1) {
        acs_dir_sym = ACS_LARROW;
    } else if (a->dir.y == -1 && a->dir.x == 0) {
        acs_dir_sym = ACS_UARROW;
    } else if (a->dir.y == 0 && a->dir.x == 1) {
        acs_dir_sym = ACS_RARROW;
    } else if (a->dir.y == 1 && a->dir.x == 0) {
        acs_dir_sym = ACS_DARROW;
    }

    // Draw ant
    attron(COLOR_PAIR(RED));
    mvaddch(y, x, acs_dir_sym);
    mvaddch(y, x + 1, acs_dir_sym);
    attroff(COLOR_PAIR(RED));
    set_point(&a->screen_pos, y, x);
}

void rotate_ant(ant *a, int tile_value, char *pattern)
{
    /*
     * Change the ant's direction based on current tile and pattern.
     *
     * param a - Pointer to the ant whose direction will be modified
     * param tile_value - Value (color) of the tile the ant is currently on
     * param pattern - String pattern that determines the ant's behavior
     */

    // Parse pattern string to find rule for given tile_value
    for (int i = 0; i < strlen(pattern); i++) {
        if (tile_value == i) {
            switch (toupper(pattern[i])) {
                case 'R':
                    // Turn clockwise 90 degrees
                    set_point(&a->dir, a->dir.x, a->dir.y * -1);
                    break;
                case 'L':
                    // Turn counter-clockwise 90 degrees
                    set_point(&a->dir, a->dir.x * -1, a->dir.y);
                    break;
                case 'U':
                    // Turn 180 degrees
                    set_point(&a->dir, a->dir.y * -1, a->dir.x * -1);
                    break;
                case 'N':
                    // Do nothing
                    break;
            }
            return;
        }
    }
}

void set_ant_dir(ant *a, char dir)
{
    /*
     * Set ant's initial direction based on given character.
     *
     * param a - Pointer to the ant whose direction will be set
     * param dir - Token used to determine new direction
     */
    if (dir == '\0') {
        return;
    }
    switch (toupper(dir)) {
        case 'L':
            // Left
            set_point(&a->dir, 0, -1);
            break;
        case 'U':
            // Up
            set_point(&a->dir, -1, 0);
            break;
        case 'R':
            // Right
            set_point(&a->dir, 0, 1);
            break;
        case 'D':
            // Down
            set_point(&a->dir, 1, 0);
            break;
    }
}
