#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdlib.h>

#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

#define BMP_HEADER_SIZE (FILE_HEADER_SIZE + INFO_HEADER_SIZE)

typedef struct Pixel {
    unsigned char r, g, b, a;
} Pixel;

typedef struct BMPHeader {
    int width;                 /* The bitmap width in pixels(signed integer) [4 bytes] */
    int height;                /* The bitmap height in pixels (signed integer) [4 bytes] */
    short int bytes_per_pixel; /* The number of bytes per pixel, which is the color depth of the image [2 bytes] */
} BMPHeader;

int get_image_size(const BMPHeader *header);

void bmp_header_to_bytes(unsigned char header_bytes[BMP_HEADER_SIZE], const BMPHeader *header);

void create_image_from_pixels(FILE *fp, Pixel **pixels, int width, int height);

Pixel **init_pixel_array(int width, int height);

void free_pixel_array(Pixel **pixels);

#endif
