#include "flags.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum FlagTypes {
    FLAG_INT = 0,
    FLAG_STR,
    FLAG_COUNT
} FlagTypes;

typedef union FlagData {
    int d;
    char *str;
} FlagData;

typedef struct Flag {
    FlagTypes type;
    FlagData data;
    FlagData def;
    const char *desc;
    const char *name;
} Flag;

static Flag flags[FLAG_CAP];
static int flag_cnt = 0;

static Flag *new_flag(FlagTypes type, const char *name, const char *desc) {
    flags[flag_cnt++] = (Flag){.type = type, .name = name, .desc = desc};
    return &(flags[flag_cnt - 1]);
}

int *new_int_flag(const char *name, int def, const char *desc) {
    Flag *new = new_flag(FLAG_INT, name, desc);
    new->def.d = def;
    new->data.d = def;
    return &(new->data.d);
}

char **new_str_flag(const char *name, char *def, const char *desc) {
    Flag *new = new_flag(FLAG_STR, name, desc);
    new->def.str = def;
    new->data.str = def;
    return &(new->data.str);
}

void parse_flags(int argc, char *argv[]) {
    Flag *prev = NULL;

    for (int i = 0; i < argc; i++) {
        if (prev != NULL) {
            switch (prev->type) {
                case FLAG_INT:
                    sscanf(argv[i], "%d", &(prev->data.d));
                    break;

                case FLAG_STR:
                    prev->data.str = argv[i];
                    break;

                default:
                    assert(0 && "Unhandled flag type");
                    break;
            }

            prev = NULL;
        }

        for (int flag_idx = 0; flag_idx < flag_cnt; flag_idx++) {

            // parse: -size
            if (1 < strlen(argv[i]) && strcmp(argv[i] + 1, flags[flag_idx].name) == 0) {
                prev = &(flags[flag_idx]);
                break;
            }
        }
    }
}
