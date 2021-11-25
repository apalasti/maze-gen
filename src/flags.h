#ifndef FLAGS_H
#define FLAGS_H

#ifndef FLAG_CAP
#define FLAG_CAP 256
#endif

int *new_int_flag(const char *name, int def, const char *desc);

char **new_str_flag(const char *name, char *def, const char *desc);

void parse_flags(int argc, char *argv[]);

#endif
