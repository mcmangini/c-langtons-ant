/*
 * langtons_ant.c
 *
 * This program implements Langton's ant in the terminal using ncurses.
 *
 * Langton's ant is a Turing machine with simple rules yet complex
 * behavior. The "ant" moves on a two-dimensional grid, changing the color
 * of tiles as it passes over them. At each step, the ant changes direction
 * based on the color of the tile it is on and a predefined rule.
 *
 * This implementation supports sixteen colors and four types of
 * direction-change (Left, Right, U-turn, None).
 * Rules are given as command-line arguments.
 *
 * Author (2025):
 *      Mark C. Mangini (mark.c.mangini@gmail.com)
 *                      (github.com/mcmangini)
 */

#include <ctype.h>
#include <limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#include "ant.h"
#include "grid.h"
#include "point.h"

enum game_state {
    RUNNING,
    PAUSED,
    GAME_OVER
};

void print_usage(void)
{
    // Print the usage message.
    char *usage_msg =
        "Usage: langtons_ant [OPTION] [PATTERN]\n"
        "Implement Langton's ant in the terminal using ncurses.\n"
        "\n"
        "Options:\n"
        "  -d <direction>           Specify a starting direction "
        "for the ant.\n"
        "                           Valid directions are:\n"
        "                             L or l (left)\n"
        "                             R or r (right)\n"
        "                             U or u (up)\n"
        "                             D or d (down)\n"
        "\n"
        "  -h, --help               Display this usage message.\n"
        "\n"
        "Pattern:\n"
        "  Pattern is a string no less than two and no more than sixteen\n"
        "  characters in length. Each character represents a rule for how\n"
        "  the ant should change direction when on a tile of a certain\n"
        "  color.\n"
        "  For example, the pattern\n"
        "\n"
        "    \"RL\"\n"
        "\n"
        "  means:\n"
        "    - Turn (R)ight on a tile of color one (black)\n"
        "    - Turn (L)eft on a tile of color two (white)\n"
        "\n"
        "  Valid pattern characters are:\n"
        "    L or l (left turn)\n"
        "    R or r (right turn)\n"
        "    U or u (u-turn)\n"
        "    N or n (none)\n"
        "\n"
        "If no arguments are provided, the direction L and the pattern\n"
        "RL are used by default.\n"
        "\n"
        "Application Controls:\n"
        "  While running, the application can be interacted with by\n"
        "  pressing certain keys.\n"
        "\n"
        "    q          - Quit\n"
        "    p          - Pause/Resume\n"
        "    r          - Restart\n"
        "    1          - Slow ant speed\n"
        "    2          - Medium ant speed\n"
        "    3          - Fast ant speed\n"
        "    Arrow keys - Pan around the grid\n"
        "    a          - Center ant's current location in terminal\n"
        "    c          - Center grid in terminal";

    printf("%s\n", usage_msg);
}

int str_in_array(char *s, char *sv[], int n)
{
    /*
     * Return 1 (true) if the array contains the string, 0 (false) otherwise.
     *
     * param s - The string to check the array for
     * param sv - The array of strings to be searched
     * param n - The size of the array sv
     * return - 1 (true) if s is found in sv, 0 (false) if not
     */
    for (int i = 0; i < n; i++) {
        if (strcmp(s, sv[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void str_toupper(char *s)
{
    /*
     * Directly change all characters in a string to uppercase.
     *
     * param s - The string to modify
     */
    while (*s) {
        *s = toupper((unsigned char) *s);
        s++;
    }
}

int main(int argc, char *argv[])
{
    enum game_state state;
    int row, col, ch, step_col;
    unsigned int step_count;
    char *quit_msg;
    char *paused_msg;
    char pattern[17];
    char dir[2];
    int grid[GRID_SIZE_Y][GRID_SIZE_X] = {{0}};
    point grid_offset = {0, 0};
    ant main_ant;

    // Parse arguments
    if (argc > 4) {
        printf("Too many arguments (\"langtons_ant --help\" for help)\n");
        return 1;
    } else if (argc == 2 && (strcmp(argv[1], "-h") == 0 ||
            strcmp(argv[1], "--help") == 0)) {
        print_usage();
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            if (dir[0] != '\0') {
                printf("Invalid arguments "
                        "(\"langtons_ant --help\" for help)\n");
                return 1;
            }
            if (i == argc - 1) {
                printf("Invalid direction "
                        "(\"langtons_ant --help\" for help)\n");
                return 1;
            }
            char *valid_d[] = {"L", "l", "U", "u", "R", "r", "D", "d"};
            if (!str_in_array(argv[i + 1], valid_d, 8)) {
                printf("Invalid direction "
                        "(\"langtons_ant --help\" for help)\n");
                return 1;
            }
            strcpy(dir, argv[i + 1]);
            str_toupper(dir);
            i++;
        } else {
            if (pattern[0] != '\0') {
                printf("Invalid arguments "
                        "(\"langtons_ant --help\" for help)\n");
                return 1;
            }
            char *s = argv[i];
            if (strlen(s) < 2 || strlen(s) > MAX_COLORS) {
                printf("Invalid pattern "
                        "(\"langtons_ant --help\" for help)\n");
                return 1;
            }
            for (int i = 0; i < strlen(s); i++) {
                if (s[i] != 'R' && s[i] != 'r' &&
                        s[i] != 'L' && s[i] != 'l' &&
                        s[i] != 'U' && s[i] != 'u' &&
                        s[i] != 'N' && s[i] != 'n') {
                    printf("Invalid pattern "
                            "(\"langtons_ant --help\" for help)\n");
                    return 1;
                }
            }
            strcpy(pattern, s);
            str_toupper(pattern);
        }
    }
    if (dir[0] == '\0') {
        strcpy(dir, "L");
    }
    if (pattern[0] == '\0') {
        strcpy(pattern, "RL");
    }

    // Initiate ncurses
    initscr();
    if (has_colors() == FALSE) {
        endwin();
        printf("Terminal does not support color\n");
        return 1;
    }
    start_color();
    init_pair(BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BLUE, COLOR_BLACK, COLOR_BLUE);
    init_pair(GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(MAGENTA, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(YELLOW, COLOR_BLACK, COLOR_YELLOW);
    init_pair(CYAN, COLOR_BLACK, COLOR_CYAN);
    init_pair(RED, COLOR_BLACK, COLOR_RED);

    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    quit_msg = "Press 'q' to quit";
    paused_msg = "--PAUSED--";
    getmaxyx(stdscr, row, col);
    step_col = strlen(dir) + strlen(pattern) + 2;

    clear();
    update_offset(&grid_offset, ZERO);
    halfdelay(5);
    set_point(&main_ant.pos, GRID_SIZE_Y / 2, GRID_SIZE_X / 2);
    set_ant_dir(&main_ant, dir[0]);
    init_grid(grid);
    render_grid(grid, row, col, &grid_offset);
    render_ant(&main_ant, row, col, &grid_offset);
    step_count = 0;
    mvprintw(row - 1, 0, "%s-%s\n", dir, pattern);
    mvprintw(row - 1, step_col, "Step: %d\n", step_count);
    mvprintw(row - 1, col - strlen(quit_msg), "%s", quit_msg);
    state = RUNNING;
    refresh();

    // Main loop
    while ((ch = getch()) != 'q') {
        // Flags
        int refresh_screen = 0;
        int reset = 0;

        // Check for these inputs regardless of state
        if (ch == KEY_RESIZE) {
            getmaxyx(stdscr, row, col);
            refresh_screen = 1;
        } else if (ch == KEY_LEFT) {
            update_offset(&grid_offset, LEFT);
            refresh_screen = 1;
        } else if (ch == KEY_UP) {
            update_offset(&grid_offset, UP);
            refresh_screen = 1;
        } else if (ch == KEY_RIGHT) {
            update_offset(&grid_offset, RIGHT);
            refresh_screen = 1;
        } else if (ch == KEY_DOWN) {
            update_offset(&grid_offset, DOWN);
            refresh_screen = 1;
        } else if (ch == 'c') {
            update_offset(&grid_offset, ZERO);
            refresh_screen = 1;
        } else if (ch == 'a') {
            center_ant(&main_ant, row, col, &grid_offset);
            refresh_screen = 1;
        } else if (state == RUNNING) {
            // Check these only when running
            if (ch == ERR) {
                update_grid(grid, &main_ant, pattern);
                if (main_ant.pos.y < 0 || main_ant.pos.y >= GRID_SIZE_Y ||
                        main_ant.pos.x < 0 ||
                        main_ant.pos.x >= GRID_SIZE_X) {
                    state = GAME_OVER;
                    continue;
                }
                if (step_count < UINT_MAX) {
                    step_count++;
                }
                refresh_screen = 1;
            } else if (ch == 'p') {
                state = PAUSED;
                mvprintw(row - 1, (col - strlen(paused_msg)) / 2, "%s",
                        paused_msg);
            } else if (ch == 'r') {
                reset = 1;
            } else if (ch == '1') {
                halfdelay(10);
            } else if (ch == '2') {
                halfdelay(5);
            } else if (ch == '3') {
                halfdelay(1);
            }
        } else if (state == PAUSED) {
            // Check these only when paused
            if (ch == 'p') {
                state = RUNNING;
                mvprintw(row - 1, (col - strlen(paused_msg)) / 2,
                        "          ");
            }
        } else if (state == GAME_OVER) {
            // Check these only when game over
            if (ch == 'r') {
                reset = 1;
            }
        }

        // Check flags
        if (reset) {
            update_offset(&grid_offset, ZERO);
            halfdelay(5);
            set_point(&main_ant.pos, GRID_SIZE_Y / 2, GRID_SIZE_X / 2);
            set_ant_dir(&main_ant, dir[0]);
            init_grid(grid);
            step_count = 0;
            state = RUNNING;
            refresh_screen = 1;
        }
        if (refresh_screen) {
            if (ch != ERR) {
                clear();
            }
            render_grid(grid, row, col, &grid_offset);
            render_ant(&main_ant, row, col, &grid_offset);
            mvprintw(row - 1, 0, "%s-%s\n", dir, pattern);
            mvprintw(row - 1, step_col, "Step: %d\n", step_count);
            if (state == PAUSED) {
                mvprintw(row - 1, (col - strlen(paused_msg)) / 2, "%s",
                        paused_msg);
            }
            mvprintw(row - 1, col - strlen(quit_msg), "%s", quit_msg);
            refresh();
        }
    }

    // Clean-up
    endwin();
    return 0;
}
