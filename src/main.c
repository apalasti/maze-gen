#include "bmp.h"
#include "flags.h"
#include "maze.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int LABIRINTH_SIZE = 5;

int main(int argc, char *argv[]) {
    int *width = new_int_flag("mw", 10, "The width of the maze");
    int *height = new_int_flag("mh", 10, "The height of the maze");

    char **out_path = new_str_flag("o", NULL, "Path to the output bmp file. If this is set it will output a picture into the specified file.");

    bool *help = new_bool_flag("h", false, "Prints out this help message");

    parse_flags(argc, argv);

    if (*help == true) {
        print_flags_usage(stdout);
        exit(EXIT_SUCCESS);
    }

    srand(time(NULL));

    if (*out_path == NULL) {
        // print maze to console
        Maze *m = init_maze(*width, *height);
        generate_maze(m, 0, 0);

        print_maze(stdout, m);

        free_maze(m);
    } else {

        // check if out_path is bmp file
        if (strstr(*out_path, ".bmp") == NULL) {
            fprintf(stderr, "ERROR: The file is not a bmp file: '%s'", *out_path);
            exit(EXIT_FAILURE);
        }

        clock_t start = clock();

        Maze *m = init_maze(*width, *height);
        generate_maze(m, 0, 0);

        // draw maze to bmp file
        FILE *fp = fopen(*out_path, "wb");
        if (fp == NULL) {
            fprintf(stderr, "ERROR: Could not open file: '%s'", strerror(errno));
            exit(EXIT_FAILURE);
        }

        Pixel **pixels = gen_pixel_arr_from_maze(m);

        create_image_from_pixels(fp, pixels,
                                 get_maze_width_in_pixels(m->width),
                                 get_maze_height_in_pixels(m->height));

        fclose(fp);
        free_pixel_array(pixels);
        free_maze(m);

        clock_t end = clock();

        fprintf(stdout, "Successfully generated maze at: '%s' (%fs)\n",
                *out_path,
                (double)(end - start) / CLOCKS_PER_SEC);
    }

    return 0;
}
