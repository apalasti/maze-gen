#include "flags.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum FlagTypes {
    FLAG_INT = 0,
    FLAG_STR,
    FLAG_BOOL,
    FLAG_COUNT
} FlagTypes;

typedef union FlagData {
    int d;
    bool b;
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
    // you have to increase FLAG_CAP because you have too much flags
    assert(flag_cnt != FLAG_CAP);
    flags[flag_cnt++] = (Flag){.type = type, .name = name, .desc = desc};
    return &(flags[flag_cnt - 1]);
}

int *new_int_flag(const char *name, int def, const char *desc) {
    Flag *new = new_flag(FLAG_INT, name, desc);
    new->def.d = def;
    new->data.d = def;
    return &(new->data.d);
}

bool *new_bool_flag(const char *name, bool def, const char *desc) {
    Flag *new = new_flag(FLAG_BOOL, name, desc);
    new->def.b = def;
    new->data.b = def;
    return &(new->data.b);
}

char **new_str_flag(const char *name, char *def, const char *desc) {
    Flag *new = new_flag(FLAG_STR, name, desc);
    new->def.str = def;
    new->data.str = def;
    return &(new->data.str);
}

void print_flags_usage(FILE *out) {
    fprintf(out, "USAGE:\n");
    for (int i = 0; i < flag_cnt; i++) {
        switch (flags[i].type) {
            case FLAG_INT:
                fprintf(out, "    -%s <int> Default: %d\n", flags[i].name, flags[i].def.d);
                fprintf(out, "        %s\n", flags[i].desc);
                break;

            case FLAG_BOOL:
                fprintf(out, "    -%s\n", flags[i].name);
                fprintf(out, "        %s\n", flags[i].desc);
                break;

            case FLAG_STR: {
                bool has_default = flags[i].def.str != NULL;
                fprintf(out, "    -%s <string> %s%s\n", flags[i].name,
                        has_default ? "Default: " : "No default",
                        has_default ? flags[i].def.str : "");
                fprintf(out, "        %s\n", flags[i].desc);
                break;
            }

            default:
                assert(0 && "Unhandled flag type");
                break;
        }
        fprintf(out, "\n");
    }
}

void parse_flags(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        bool recognized = false;
        for (int flag_idx = 0; flag_idx < flag_cnt; flag_idx++) {

            // parse: -flag_name
            if (1 < strlen(argv[i]) && strcmp(argv[i] + 1, flags[flag_idx].name) == 0) {
                recognized = true;
                Flag *flag = &(flags[flag_idx]);

                // check if we are at the end of the flags
                if (flag->type != FLAG_BOOL && i == argc - 1) {
                    fprintf(stderr, "ERROR: You did not provide a flag value for: '%s'\n", argv[i]);
                    print_flags_usage(stderr);
                    exit(EXIT_FAILURE);
                }

                // parse flags
                switch (flag->type) {
                    case FLAG_INT:
                        // if sscanf did not successfully read in a value
                        if (sscanf(argv[i + 1], "%d", &(flag->data.d)) != 1) {
                            fprintf(stderr, "ERROR: Could not parse flag value: '%s', for flag '%s'\n", argv[i + 1], argv[i]);
                            print_flags_usage(stderr);
                            exit(EXIT_FAILURE);
                        }
                        break;

                    case FLAG_BOOL:
                        flag->data.b = true;
                        break;

                    case FLAG_STR:
                        flag->data.str = argv[i + 1];
                        break;

                    default:
                        assert(0 && "Unhandled flag type");
                        break;
                }

                // we found the matching flag so break out from the loop
                i++;
                break;
            }
        }
        if (recognized == false) {
            fprintf(stderr, "ERROR: Unrecognized flag: '%s'\n", argv[i]);
            print_flags_usage(stderr);
            exit(EXIT_FAILURE);
        }
    }
}
