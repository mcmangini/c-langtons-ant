/*
 * grid.c
 */

#include <ncurses.h>
#include <string.h>

#include "grid.h"

void change_tile(int grid[][GRID_SIZE_X], point *p, int pattern_len)
{
    /*
     * Increment the value (color) of a tile on the grid.
     *
     * param grid - The 2D grid array that holds the tile
     * param p - Pointer to the coordinates of the tile on the grid
     * param pattern_len - Length of the pattern that describes ant behavior
     *   Determines maximum value of tiles
     */
    grid[p->y][p->x] = (grid[p->y][p->x] + 1) % pattern_len;
}

void init_grid(int grid[][GRID_SIZE_X])
{
    /*
     * Initialize all tiles on grid with a value of 0 (color Black).
     *
     * param grid - Grid to be initialized
     */
    for (int i = 0; i < GRID_SIZE_Y; i++) {
        for (int j = 0; j < GRID_SIZE_X; j++) {
            grid[i][j] = 0;
        }
    }
}

void render_grid(int grid[][GRID_SIZE_X], int row, int col,
        point *grid_offset)
{
    /*
     * Draw the grid on the terminal.
     *
     * param grid - Grid to render
     * param row - Number of rows (lines) in the terminal
     * param col - Number of columns in the terminal
     * param grid_offset - Pointer to the grid offset
     */
    point start_p = {(row - GRID_SIZE_Y) / 2, (col - GRID_SIZE_X * 2) / 2};

    start_p = add_points(&start_p, grid_offset);
    // Make sure we start on an even column
    if (start_p.x % 2 != 0) {
        start_p.x += 1;
    }
    for (int i = 0; i < GRID_SIZE_Y; i++) {
        for (int j = 0; j < GRID_SIZE_X; j++) {
            char s[3] = "  ";
            /*
             * ncurses has only 8 bg colors, so use characters to indicate
             * colors 8 - 15
             */
            if (grid[i][j] > 7) {
                strcpy(s, "##");
            }
            // Render tile
            attron(COLOR_PAIR(grid[i][j] % 8));
            mvprintw(start_p.y + i, start_p.x + j * 2, "%s", s);
            attroff(COLOR_PAIR(grid[i][j]));
        }
    }
}

void update_grid(int grid[][GRID_SIZE_X], ant *a, char *pattern)
{
    /* Update grid based on ant's position and direction.
     *
     * param grid - Grid to be updated
     * param a - Pointer to ant
     * param pattern - String that describes ant behavior
     */
    point prev_pos, pos;
    prev_pos = a->pos;

    // Update ant direction
    rotate_ant(a, grid[a->pos.y][a->pos.x], pattern);
    // Update ant position
    pos = add_points(&a->pos, &a->dir);
    set_point(&a->pos, pos.y, pos.x);
    // Change color of tile at previous position
    change_tile(grid, &prev_pos, strlen(pattern));
}

void update_offset(point *grid_offset, enum offset_direction dir)
{
    /*
     * Update grid offset based on given direction.
     *
     * param grid_offset - Grid offset to update
     * param dir - Direction in which offset will be changed
     */
    switch (dir) {
        case LEFT:
            // Always move 2 columns at a time
            grid_offset->x += 2;
            break;
        case UP:
            grid_offset->y += 1;
            break;
        case RIGHT:
            // Always move 2 columns at a time
            grid_offset->x -= 2;
            break;
        case DOWN:
            grid_offset->y -= 1;
            break;
        case ZERO:
            // Reset offset to 0
            set_point(grid_offset, 0, 0);
            break;
    }
}
