#ifndef FLAGS_H
#define FLAGS_H

#include <stdbool.h>
#include <stdio.h>

#ifndef FLAG_CAP
#define FLAG_CAP 256
#endif

int *new_int_flag(const char *name, int def, const char *desc);

char **new_str_flag(const char *name, char *def, const char *desc);

bool *new_bool_flag(const char *name, bool def, const char *desc);

bool parse_flags(int argc, char *argv[]);

void print_flags_usage(FILE *out);

void print_flag_error(FILE *out);

#endif
