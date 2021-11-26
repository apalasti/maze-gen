#ifndef MAZE_H
#define MAZE_H

#include "bmp.h"
#include <stdbool.h>
#include <stdio.h>

#define WALL "██"
#define SPACE "  "

enum DIRECTIONS {
    TOP = 0,
    LEFT,
    RIGHT,
    BOTTOM,
    DIRECTION_COUNT
};

typedef struct Cell {
    bool is_visited;
    bool walls[DIRECTION_COUNT];
} Cell;

typedef struct Maze {
    Cell **cells;
    int width, height;
} Maze;

int get_maze_width_in_pixels(int width, int block_size);

int get_maze_height_in_pixels(int height, int block_size);

Pixel **gen_pixel_arr_from_maze(const Maze *m, int block_size);

Maze *init_maze(int width, int height);

void generate_maze(Maze *m, int start_x, int start_y);

void clear_maze(Maze *m);

void print_maze(FILE *out, Maze *m);

void free_maze(Maze *m);

#endif
