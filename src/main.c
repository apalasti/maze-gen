#include "bmp.h"
#include "maze.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FLAG_CAP 5
#include "flags.h"

int main(int argc, char *argv[]) {
    int *width = new_int_flag("mw", 10, "The width of the maze");
    int *height = new_int_flag("mh", 10, "The height of the maze");

    int *block_size = new_int_flag("bs", 10, "The size of a square in the maze in pixels");

    char **out_path = new_str_flag("o", NULL, "Path to the output bmp file. If this is set it will output a picture into the specified file.");

    bool *help = new_bool_flag("h", false, "Prints out this help message and exits with 0");

    if (parse_flags(argc, argv) == false) {
        print_flag_error(stderr);
        print_flag_usage(stderr);
        exit(EXIT_FAILURE);
    }

    if (*help == true) {
        print_flag_usage(stdout);
        exit(EXIT_SUCCESS);
    }

    assert(*width > 0);
    assert(*height > 0);
    assert(*block_size > 0);

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

        Pixel **pixels = gen_pixel_arr_from_maze(m, *block_size);

        create_image_from_pixels(fp, pixels,
                                 get_maze_width_in_pixels(m->width, *block_size),
                                 get_maze_height_in_pixels(m->height, *block_size));
        size_t file_size = ftell(fp);

        fclose(fp);
        free_pixel_array(pixels);
        free_maze(m);

        clock_t end = clock();
        double duration = (double)(end - start) / CLOCKS_PER_SEC;

        fprintf(stdout, "Successfully generated maze at: '%s' (%fs, %g MB)\n",
                *out_path,
                duration,
                file_size / (1000.0 * 1000.0));
    }

    return 0;
}
