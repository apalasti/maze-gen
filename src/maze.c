#include "maze.h"
#include "bmp.h"
#include "util.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const int MOVES[DIRECTION_COUNT][2] = {
    [TOP] = {0, -1},
    [LEFT] = {-1, 0},
    [RIGHT] = {1, 0},
    [BOTTOM] = {0, 1},
};

int get_maze_width_in_pixels(int width) {
    return width * 2 + 1;
}

int get_maze_height_in_pixels(int height) {
    return height * 2 + 1;
}

Pixel **gen_pixel_arr_from_maze(const Maze *m) {
    Pixel wall = {.r = 0, .g = 0, .b = 0, .a = 255};        // black
    Pixel space = {.r = 255, .g = 255, .b = 255, .a = 255}; // white

    Pixel **pixels = init_pixel_array(get_maze_width_in_pixels(m->width),
                                      get_maze_height_in_pixels(m->height));

    for (int y = 0; y < m->height; y++) {
        int y_in_pixels = 2 * y + 1;

        for (int x = 0; x < m->width; x++) {
            int x_in_pixels = 2 * x + 1;

            pixels[y_in_pixels][x_in_pixels] = space;

            // draw the four walls around cell
            for (int dir = TOP; dir < DIRECTION_COUNT; dir++) {
                bool is_wall = m->cells[y][x].walls[dir];
                int move_x = x_in_pixels + MOVES[dir][0],
                    move_y = y_in_pixels + MOVES[dir][1];

                pixels[move_y][move_x] = is_wall ? wall : space;
            }

            // fill diagonal corners
            pixels[y_in_pixels - 1][x_in_pixels - 1] = wall;
            pixels[y_in_pixels - 1][x_in_pixels + 1] = wall;
            pixels[y_in_pixels + 1][x_in_pixels - 1] = wall;
            pixels[y_in_pixels + 1][x_in_pixels + 1] = wall;
        }
    }

    return pixels;
}

void generate_maze(Maze *m, int start_x, int start_y) {
    assert(0 <= start_y && start_y < m->height);
    assert(0 <= start_x && start_x < m->width);

    m->cells[start_y][start_x].is_visited = true;

    int directions[DIRECTION_COUNT];
    for (int i = TOP; i < DIRECTION_COUNT; i++)
        directions[i] = i;

    // shuffle directions
    shuffle(directions, DIRECTION_COUNT);

    for (int i = 0; i < DIRECTION_COUNT; i++) {
        int dir = directions[i];
        int move_x = clamp(start_x + MOVES[dir][0], 0, m->width),
            move_y = clamp(start_y + MOVES[dir][1], 0, m->height);

        if (m->cells[move_y][move_x].is_visited == false) {
            // delete walls so the two cells are connected
            m->cells[start_y][start_x].walls[dir] = false;
            // DIRECTION_COUNT - 1 - dir = inverse direction
            m->cells[move_y][move_x].walls[DIRECTION_COUNT - 1 - dir] = false;
            generate_maze(m, move_x, move_y);
        }
    }
}

Maze *init_maze(int width, int height) {
    assert(0 < width && 0 < height);
    Maze *m = malloc(sizeof(Maze));
    check_malloc(m);

    m->width = width;
    m->height = height;

    // allocate 2d array
    m->cells = (Cell **)malloc(height * sizeof(Cell *));
    check_malloc(m->cells);
    m->cells[0] = (Cell *)malloc(width * height * sizeof(Cell));
    check_malloc(m->cells[0]);
    for (int i = 1; i < height; i++) {
        m->cells[i] = m->cells[0] + width * i;
    }

    clear_maze(m);

    return m;
}

void print_maze(FILE *out, Maze *m) {
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            fprintf(out, "%s%s", WALL, m->cells[y][x].walls[TOP] ? WALL : SPACE);
        }
        printf("%s\n", WALL);
        for (int x = 0; x < m->width; x++) {
            fprintf(out, "%s%s", m->cells[y][x].walls[LEFT] ? WALL : SPACE, SPACE);
        }
        printf("%s\n", WALL);
    }
    for (int i = 0; i < m->width; i++) {
        fprintf(out, "%s%s", WALL, WALL);
    }
    printf("%s\n", WALL);
}

void clear_maze(Maze *m) {
    for (int i = 0; i < m->height; i++) {
        for (int j = 0; j < m->width; j++) {
            m->cells[i][j].is_visited = false;
            for (int direction = TOP; direction < DIRECTION_COUNT; direction++) {
                m->cells[i][j].walls[direction] = true;
            }
        }
    }
}

void free_maze(Maze *m) {
    free(m->cells[0]);
    free(m->cells);
    free(m);
}
