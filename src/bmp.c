#include "bmp.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int get_image_size(const BMPHeader *header) {
    return header->width * header->height * header->bytes_per_pixel;
}

void write_bytes(unsigned char bytes[], int val, size_t len) {
    for (size_t i = 0; i < len; i++) {
        bytes[i] = (unsigned char)(val >> 8 * i);
    }
}

void bmp_header_to_bytes(unsigned char header_bytes[BMP_HEADER_SIZE],
                         const BMPHeader *header) {
    int image_size = get_image_size(header);
    // clear header
    for (int i = 0; i < BMP_HEADER_SIZE; i++)
        header_bytes[i] = 0;

    // File Header
    // header field
    write_bytes(&header_bytes[0], 'B', 1);
    write_bytes(&header_bytes[1], 'M', 1);

    write_bytes(&header_bytes[2], BMP_HEADER_SIZE + image_size, 4); // The size of the bmp file in bytes
    write_bytes(&header_bytes[10], BMP_HEADER_SIZE, 4);             // Starting address of pixel array

    // Info Header
    write_bytes(&header_bytes[14], INFO_HEADER_SIZE, 4);            // The size of info header
    write_bytes(&header_bytes[18], header->width, 4);               // The bitmap width in pixels
    write_bytes(&header_bytes[22], header->height, 4);              // The bitmap height in pixels
    write_bytes(&header_bytes[26], 1, 2);                           // Number of color planes
    write_bytes(&header_bytes[28], header->bytes_per_pixel * 8, 2); // The number of bits per pixel, which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.
    write_bytes(&header_bytes[34], image_size, 4);                  // This is the size of the raw bitmap data
}

/**
 * @brief Write pixels to bmp file
 * 
 * @param fp Has to be a bmp file and the file has to be opened with "wb" flags
 * @param pixels The array of pixels
 * @param width Width of the pixels
 * @param height Height of the pixels
 */
void create_image_from_pixels(FILE *fp, Pixel **pixels, int width, int height) {
    BMPHeader h = {
        .width = width,
        .height = height,
        .bytes_per_pixel = 4,
    };
    unsigned char header_bytes[BMP_HEADER_SIZE];
    bmp_header_to_bytes(header_bytes, &h);

    int image_size = get_image_size(&h);

    unsigned char *pixel_array = malloc(image_size);
    check_malloc(pixel_array);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int p = ((height - row - 1) * width + col) * 4;
            pixel_array[p + 3] = pixels[row][col].a; // alpha
            pixel_array[p + 2] = pixels[row][col].r; // red
            pixel_array[p + 1] = pixels[row][col].g; //green
            pixel_array[p + 0] = pixels[row][col].b; //blue
        }
    }

    fwrite(header_bytes, 1, BMP_HEADER_SIZE, fp);
    fwrite(pixel_array, 1, image_size, fp);

    free(pixel_array);
}

Pixel **init_pixel_array(int width, int height) {
    Pixel **pixels = malloc(height * sizeof(Pixel *));
    check_malloc(pixels);
    pixels[0] = malloc(width * height * sizeof(Pixel));
    check_malloc(pixels[0]);

    for (int i = 1; i < height; i++)
        pixels[i] = pixels[0] + i * width;

    return pixels;
}

void free_pixel_array(Pixel **pixels) {
    free(pixels[0]);
    free(pixels);
}
