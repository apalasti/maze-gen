#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int clamp(int val, int min, int max) {
    if (val < min)
        return min;

    if (max - 1 < val)
        return max - 1;

    return val;
}

void shuffle(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        int rnd = rand() % size;
        int tmp = arr[i];
        arr[i] = arr[rnd];
        arr[rnd] = tmp;
    }
}

void check_malloc(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "ERROR: Could not allocate memory\n");
        exit(EXIT_FAILURE);
    }
}
