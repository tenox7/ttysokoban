#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Include the embedded levels and game definitions */
#include "embedded_levels.h"
#include "levels.h"

/* Color pairs */
#define PAIR_WALL      1  /* WHITE on BLUE */
#define PAIR_PLAYER    2  /* BLACK on YELLOW */
#define PAIR_BOX       3  /* BLACK on RED */
#define PAIR_GOAL      4  /* BLACK on YELLOW */
#define PAIR_BOX_GOAL  5  /* WHITE on MAGENTA */
#define PAIR_FLOOR     6  /* BLACK on CYAN */
#define PAIR_DEFAULT   7  /* WHITE on BLACK */

/* Display characters (easy to change) */
#define DISP_WALL '#'
#define DISP_PLAYER '@'
#define DISP_BOX '#'
#define DISP_BOX_ON_GOAL '0'
#define DISP_GOAL 'O'

/* Game state */
typedef struct {
    char** map;
    int width;
    int height;
    int player_x;
    int player_y;
    int boxes_total;
    int boxes_on_goal;
    char* level_name;
    int use_ascii_borders;
    int use_colors;
} Game;

/* Global variables */
int current_level = 0;  /* Current level index */
int num_levels = 0;     /* Total number of levels */

/* Function prototypes */
void init_curses(void);
char** load_level(int level_index, int* width, int* height, int* player_x, int* player_y, int* boxes);
void free_map(char** map, int height);
void draw_map(const Game* game);
int move_player(Game* game, int dx, int dy);
void show_help(const char* program_name);

/* Function to display help */
void show_help(const char* program_name) {
    printf("TTY Sokoban - a terminal-based Sokoban game\n");
    printf("Usage: %s [options]\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --help     Show this help message and exit\n");
    printf("  -a, --ascii    Use ASCII characters for walls instead of box drawing characters\n");
    printf("  -b, -bw        Black and white mode (disable colors)\n");
    printf("\nControls:\n");
    printf("  Arrow keys, WASD, or HJKL    Move player\n");
    printf("  R                            Restart current level\n");
    printf("  N                            Next level\n");
    printf("  P                            Previous level\n");
    printf("  Q                            Quit game\n");
}

/* Main function */
int main(int argc, char *argv[]) {
    /* Game state */
    Game game;
    int game_running = 1;
    int level_complete = 0;
    int ch;

    /* Initialize level variables */
    current_level = 0;
    num_levels = NUM_EMBEDDED_LEVELS;

    /* Check for command line flags */
    game.use_ascii_borders = 0;
    game.use_colors = 1;  /* Colors enabled by default */

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ascii") == 0) {
            game.use_ascii_borders = 1;
        }
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "-bw") == 0) {
            game.use_colors = 0;  /* Disable colors */
        }
    }

    /* Initialize ncurses */
    init_curses();

    if (num_levels == 0) {
        endwin();
        fprintf(stderr, "No embedded levels found.\n");
        return EXIT_FAILURE;
    }

    /* Load first level */
    game.map = load_level(current_level, &game.width, &game.height,
                          &game.player_x, &game.player_y, &game.boxes_total);
    game.boxes_on_goal = 0;
    game.level_name = strdup(embedded_levels[current_level].name);

    /* Game loop */
    while (game_running) {
        /* Draw the map */
        clear();
        draw_map(&game);

        /* Status info is now shown in draw_map */

        /* Check if level is complete */
        if (game.boxes_on_goal == game.boxes_total) {
            level_complete = 1;
            mvprintw(game.height + 6, 0, "Level complete! Press 'n' for next level.");
        }

        /* Get input */
        ch = getch();

        /* Process input */
        switch (ch) {
            case KEY_UP:
            case 'w':
            case 'W':
            case 'k':
            case 'K':
                move_player(&game, 0, -1);
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
            case 'j':
            case 'J':
                move_player(&game, 0, 1);
                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
            case 'h':
            case 'H':
                move_player(&game, -1, 0);
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
            case 'l':
            case 'L':
                move_player(&game, 1, 0);
                break;
            case 'r':
                /* Restart level */
                free_map(game.map, game.height);
                game.map = load_level(current_level, &game.width, &game.height,
                                      &game.player_x, &game.player_y, &game.boxes_total);
                game.boxes_on_goal = 0;
                break;
            case 'n':
                /* Next level */
                if (current_level < num_levels - 1 || level_complete) {
                    free_map(game.map, game.height);
                    free(game.level_name);

                    if (level_complete) {
                        current_level = (current_level + 1) % num_levels;
                        level_complete = 0;
                    } else {
                        current_level++;
                    }

                    game.map = load_level(current_level, &game.width, &game.height,
                                         &game.player_x, &game.player_y, &game.boxes_total);
                    game.boxes_on_goal = 0;
                    game.level_name = strdup(embedded_levels[current_level].name);
                }
                break;
            case 'p':
                /* Previous level */
                if (current_level > 0) {
                    free_map(game.map, game.height);
                    free(game.level_name);
                    current_level--;
                    game.map = load_level(current_level, &game.width, &game.height,
                                         &game.player_x, &game.player_y, &game.boxes_total);
                    game.boxes_on_goal = 0;
                    game.level_name = strdup(embedded_levels[current_level].name);
                    level_complete = 0;
                }
                break;
            case 'q':
                /* Quit */
                game_running = 0;
                break;
        }
    }

    /* Clean up */
    free_map(game.map, game.height);
    free(game.level_name);
    endwin();

    return EXIT_SUCCESS;
}

/* Initialize ncurses */
void init_curses(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    /* Initialize colors */
    if (has_colors()) {
        start_color();
        init_pair(PAIR_WALL, COLOR_WHITE, COLOR_BLUE);
        init_pair(PAIR_PLAYER, COLOR_BLACK, COLOR_GREEN);
        init_pair(PAIR_BOX, COLOR_BLACK, COLOR_RED);
        init_pair(PAIR_GOAL, COLOR_RED, COLOR_CYAN);
        init_pair(PAIR_BOX_GOAL, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(PAIR_FLOOR, COLOR_BLACK, COLOR_YELLOW);
        init_pair(PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    }
}

/* Load a level from embedded data */
char** load_level(int level_index, int* width, int* height, int* player_x, int* player_y, int* boxes) {
    if (level_index < 0 || level_index >= NUM_EMBEDDED_LEVELS) {
        endwin();
        fprintf(stderr, "Invalid level index: %d\n", level_index);
        exit(EXIT_FAILURE);
    }

    const char* level_data = embedded_levels[level_index].data;
    const char* ptr = level_data;
    int max_width = 0;
    int num_lines = 0;
    char** map;
    int line_idx;
    int i, j, len;

    /* Count the number of lines and find the longest line */
    const char* line_start = ptr;
    while (*ptr) {
        if (*ptr == '\n') {
            len = ptr - line_start;
            if (len > max_width) {
                max_width = len;
            }
            num_lines++;
            line_start = ptr + 1;
        }
        ptr++;
    }

    /* Check for last line without newline */
    if (ptr > line_start) {
        len = ptr - line_start;
        if (len > max_width) {
            max_width = len;
        }
        num_lines++;
    }

    /* Allocate memory for the map */
    map = (char**)malloc(num_lines * sizeof(char*));
    for (i = 0; i < num_lines; i++) {
        map[i] = (char*)malloc((max_width + 1) * sizeof(char));
        /* Initialize with spaces */
        for (j = 0; j < max_width; j++) {
            map[i][j] = EMPTY;
        }
        map[i][max_width] = '\0';
    }

    /* Parse the map data */
    *boxes = 0;
    line_idx = 0;
    ptr = level_data;

    line_start = ptr;
    while (*ptr) {
        if (*ptr == '\n' || *(ptr + 1) == '\0') {
            /* If it's the last character and not a newline, include it */
            len = ptr - line_start;
            if (*ptr != '\n' && *(ptr + 1) == '\0') {
                len++;
            }

            /* Copy the line */
            for (i = 0; i < len; i++) {
                map[line_idx][i] = line_start[i];

                /* Count boxes and find player position */
                if (line_start[i] == BOX || line_start[i] == BOX_ON_GOAL) {
                    (*boxes)++;
                }
                if (line_start[i] == PLAYER) {
                    *player_x = i;
                    *player_y = line_idx;
                }
                if (line_start[i] == PLAYER_ON_GOAL) {
                    *player_x = i;
                    *player_y = line_idx;
                }
            }

            line_idx++;
            line_start = ptr + 1;
        }
        ptr++;
    }

    /* Set the width and height */
    *width = max_width;
    *height = num_lines;

    return map;
}

/* Free the map memory */
void free_map(char** map, int height) {
    int i;

    for (i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}

/* Draw the map */
void draw_map(const Game* game) {
    int y, x;
    char ch;
    int start_y, start_x;
    int screen_width, screen_height;

    /* Get terminal dimensions */
    getmaxyx(stdscr, screen_height, screen_width);

    /* Calculate centering offsets */
    start_y = (screen_height - game->height) / 2;
    start_x = (screen_width - game->width) / 2;

    /* Make sure we don't go off screen */
    start_y = (start_y < 0) ? 0 : start_y;
    start_x = (start_x < 0) ? 0 : start_x;

    /* Set default colors */
    if (game->use_colors && has_colors()) {
        attron(COLOR_PAIR(PAIR_DEFAULT));
    }

    /* Clear the screen */
    clear();

    /* First clear the background for the entire map area */
    for (y = 0; y < game->height; y++) {
        for (x = 0; x < game->width; x++) {
            mvaddch(start_y + y, start_x + x, ' ');
        }
    }

    /* Then draw the map elements */
    for (y = 0; y < game->height; y++) {
        for (x = 0; x < game->width; x++) {
            ch = game->map[y][x];

            /* Apply colors if enabled */
            if (game->use_colors && has_colors()) {
                switch (ch) {
                    case WALL:
                        attron(COLOR_PAIR(PAIR_WALL));
                        break;
                    case PLAYER:
                    case PLAYER_ON_GOAL:
                        attron(COLOR_PAIR(PAIR_PLAYER));
                        break;
                    case BOX:
                        attron(COLOR_PAIR(PAIR_BOX));
                        break;
                    case GOAL:
                        attron(COLOR_PAIR(PAIR_GOAL));
                        break;
                    case BOX_ON_GOAL:
                        attron(COLOR_PAIR(PAIR_BOX_GOAL));
                        break;
                    case EMPTY:
                        attron(COLOR_PAIR(PAIR_FLOOR));
                        break;
                    default:
                        attron(COLOR_PAIR(PAIR_DEFAULT));
                        break;
                }
            }

            /* Draw map elements */
            if (ch == WALL) {
                /* Use box drawing characters instead of reverse video */
                int up = (y > 0 && game->map[y-1][x] == WALL);
                int down = (y < game->height-1 && game->map[y+1][x] == WALL);
                int left = (x > 0 && game->map[y][x-1] == WALL);
                int right = (x < game->width-1 && game->map[y][x+1] == WALL);

                if (game->use_ascii_borders) {
                    /* ASCII box characters */
                    if (up && down && left && right) mvaddch(start_y + y, start_x + x, '+');
                    else if (up && down && left) mvaddch(start_y + y, start_x + x, '+');
                    else if (up && down && right) mvaddch(start_y + y, start_x + x, '+');
                    else if (up && left && right) mvaddch(start_y + y, start_x + x, '+');
                    else if (down && left && right) mvaddch(start_y + y, start_x + x, '+');
                    else if (up && down) mvaddch(start_y + y, start_x + x, '|');
                    else if (left && right) mvaddch(start_y + y, start_x + x, '-');
                    else if (up && right) mvaddch(start_y + y, start_x + x, '+');
                    else if (up && left) mvaddch(start_y + y, start_x + x, '+');
                    else if (down && right) mvaddch(start_y + y, start_x + x, '+');
                    else if (down && left) mvaddch(start_y + y, start_x + x, '+');
                    else if (up) mvaddch(start_y + y, start_x + x, '|');
                    else if (down) mvaddch(start_y + y, start_x + x, '|');
                    else if (left) mvaddch(start_y + y, start_x + x, '-');
                    else if (right) mvaddch(start_y + y, start_x + x, '-');
                    else mvaddch(start_y + y, start_x + x, '+');
                } else {
                    /* Box drawing characters */
                    if (up && down && left && right) mvaddch(start_y + y, start_x + x, ACS_PLUS);
                    else if (up && down && left) mvaddch(start_y + y, start_x + x, ACS_RTEE);
                    else if (up && down && right) mvaddch(start_y + y, start_x + x, ACS_LTEE);
                    else if (up && left && right) mvaddch(start_y + y, start_x + x, ACS_BTEE);
                    else if (down && left && right) mvaddch(start_y + y, start_x + x, ACS_TTEE);
                    else if (up && down) mvaddch(start_y + y, start_x + x, ACS_VLINE);
                    else if (left && right) mvaddch(start_y + y, start_x + x, ACS_HLINE);
                    else if (up && right) mvaddch(start_y + y, start_x + x, ACS_LLCORNER);
                    else if (up && left) mvaddch(start_y + y, start_x + x, ACS_LRCORNER);
                    else if (down && right) mvaddch(start_y + y, start_x + x, ACS_ULCORNER);
                    else if (down && left) mvaddch(start_y + y, start_x + x, ACS_URCORNER);
                    else if (up) mvaddch(start_y + y, start_x + x, ACS_VLINE);
                    else if (down) mvaddch(start_y + y, start_x + x, ACS_VLINE);
                    else if (left) mvaddch(start_y + y, start_x + x, ACS_HLINE);
                    else if (right) mvaddch(start_y + y, start_x + x, ACS_HLINE);
                    else mvaddch(start_y + y, start_x + x, ACS_PLUS);
                }
            } else if (ch == PLAYER || ch == PLAYER_ON_GOAL) {
                mvaddch(start_y + y, start_x + x, DISP_PLAYER);
            } else if (ch == BOX) {
                mvaddch(start_y + y, start_x + x, DISP_BOX);
            } else if (ch == BOX_ON_GOAL) {
                mvaddch(start_y + y, start_x + x, DISP_BOX_ON_GOAL);
            } else if (ch == GOAL) {
                mvaddch(start_y + y, start_x + x, DISP_GOAL);
            } else {
                mvaddch(start_y + y, start_x + x, ch);
            }

            /* Reset colors for this cell */
            if (game->use_colors && has_colors()) {
                attroff(COLOR_PAIR(PAIR_WALL));
                attroff(COLOR_PAIR(PAIR_PLAYER));
                attroff(COLOR_PAIR(PAIR_BOX));
                attroff(COLOR_PAIR(PAIR_GOAL));
                attroff(COLOR_PAIR(PAIR_BOX_GOAL));
                attroff(COLOR_PAIR(PAIR_FLOOR));
                attron(COLOR_PAIR(PAIR_DEFAULT));
            }
        }
    }

    /* Reset to default color for status text */
    if (game->use_colors && has_colors()) {
        attrset(COLOR_PAIR(PAIR_DEFAULT));
    }

    /* Show status info in centered position */
    mvprintw(start_y + game->height + 1, start_x, "Level: %s (%d/%d)",
             game->level_name, current_level + 1, num_levels);
    mvprintw(start_y + game->height + 2, start_x, "Boxes: %d/%d", game->boxes_on_goal, game->boxes_total);

    /* Only display legend if there's enough screen space */
    if (start_y + game->height + 4 < screen_height) {
        mvprintw(start_y + game->height + 3, start_x, "Arrows/WASD/hjkl move");
        mvprintw(start_y + game->height + 4, start_x, "[R]estart, [N]ext, [P]rev, [Q]uit");
    }

    refresh();
}

/* Move the player */
int move_player(Game* game, int dx, int dy) {
    int new_x = game->player_x + dx;
    int new_y = game->player_y + dy;
    int box_new_x, box_new_y;
    char current_box;
    char current_pos;

    /* Check if new position is within bounds */
    if (new_x < 0 || new_x >= game->width || new_y < 0 || new_y >= game->height) {
        return 0;
    }

    /* Check if new position is a wall */
    if (game->map[new_y][new_x] == WALL) {
        return 0;
    }

    /* Check if new position has a box */
    if (game->map[new_y][new_x] == BOX || game->map[new_y][new_x] == BOX_ON_GOAL) {
        box_new_x = new_x + dx;
        box_new_y = new_y + dy;

        /* Check if box can be pushed */
        if (box_new_x < 0 || box_new_x >= game->width || box_new_y < 0 || box_new_y >= game->height) {
            return 0;
        }

        /* Check if the position the box would be pushed to is free */
        if (game->map[box_new_y][box_new_x] != EMPTY && game->map[box_new_y][box_new_x] != GOAL) {
            return 0;
        }

        /* Push the box */
        current_box = game->map[new_y][new_x];

        /* If the box is on a goal, reveal the goal when the box is moved */
        if (current_box == BOX_ON_GOAL) {
            game->map[new_y][new_x] = GOAL;
            game->boxes_on_goal--;
        } else {
            game->map[new_y][new_x] = EMPTY;
        }

        /* If the box is pushed onto a goal, mark it as such */
        if (game->map[box_new_y][box_new_x] == GOAL) {
            game->map[box_new_y][box_new_x] = BOX_ON_GOAL;
            game->boxes_on_goal++;
        } else {
            game->map[box_new_y][box_new_x] = BOX;
        }
    }

    /* Move the player */
    current_pos = (game->map[game->player_y][game->player_x] == PLAYER_ON_GOAL) ? GOAL : EMPTY;
    game->map[game->player_y][game->player_x] = current_pos;

    if (game->map[new_y][new_x] == GOAL) {
        game->map[new_y][new_x] = PLAYER_ON_GOAL;
    } else {
        game->map[new_y][new_x] = PLAYER;
    }

    game->player_x = new_x;
    game->player_y = new_y;

    return 1;
}

