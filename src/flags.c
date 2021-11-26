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

typedef enum FlagErrors {
    FLAG_NO_ERROR = 0,
    FLAG_ERROR_UNKNOWN,
    FLAG_ERROR_NO_VALUE,
    FLAG_ERROR_INVALID_VALUE,
} FlagErrors;

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

typedef struct FlagContext {
    Flag flags[FLAG_CAP];
    int flag_cnt;

    FlagErrors error_type;
    char *errored_flag; /* The name of the flag which errored */
} FlagContext;

static FlagContext flag_context = {
    .flag_cnt = 0,
    .error_type = FLAG_NO_ERROR,
};

static Flag *new_flag(FlagTypes type, const char *name, const char *desc) {
    // you have to increase FLAG_CAP because you have too much flags
    assert(flag_context.flag_cnt != FLAG_CAP);
    flag_context.flags[flag_context.flag_cnt++] = (Flag){.type = type, .name = name, .desc = desc};
    return &(flag_context.flags[flag_context.flag_cnt - 1]);
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
    for (int i = 0; i < flag_context.flag_cnt; i++) {
        Flag cur_flag = flag_context.flags[i];
        switch (cur_flag.type) {
            case FLAG_INT:
                fprintf(out, "    -%s <int> Default: %d\n", cur_flag.name, cur_flag.def.d);
                fprintf(out, "        %s\n", cur_flag.desc);
                break;

            case FLAG_BOOL:
                fprintf(out, "    -%s Default: %s\n", cur_flag.name, cur_flag.def.b ? "true" : "false");
                fprintf(out, "        %s\n", cur_flag.desc);
                break;

            case FLAG_STR: {
                bool has_default = cur_flag.def.str != NULL;
                fprintf(out, "    -%s <string> %s%s\n", cur_flag.name,
                        has_default ? "Default: " : "No default",
                        has_default ? cur_flag.def.str : "");
                fprintf(out, "        %s\n", cur_flag.desc);
                break;
            }

            default:
                assert(0 && "Unhandled flag type");
                break;
        }
        fprintf(out, "\n");
    }
}

static char *next_arg(int *argc, char **argv[]) {
    assert(*argc > 0);
    char *result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

bool parse_flags(int argc, char *argv[]) {
    // the first arg is always the execution so we can throw that away
    next_arg(&argc, &argv);

    while (argc > 0) {
        char *arg = next_arg(&argc, &argv);
        bool found = false;
        for (int i = 0; i < flag_context.flag_cnt; i++) {
            Flag *flag = &(flag_context.flags[i]);

            // parse: -flag_name
            if (1 < strlen(arg) && strcmp(arg + 1, flag->name) == 0) {

                // parse the flag
                switch (flag->type) {
                    case FLAG_INT: {
                        if (argc == 0) {
                            flag_context.error_type = FLAG_ERROR_NO_VALUE;
                            flag_context.errored_flag = arg;
                            return false;
                        }

                        char *val = next_arg(&argc, &argv);
                        // if sscanf did not successfully read in a value
                        if (sscanf(val, "%d", &(flag->data.d)) != 1) {
                            flag_context.error_type = FLAG_ERROR_INVALID_VALUE;
                            flag_context.errored_flag = arg;
                            return false;
                        }
                        break;
                    }

                    case FLAG_BOOL: {
                        flag->data.b = true;
                        break;
                    }

                    case FLAG_STR: {
                        if (argc == 0) {
                            flag_context.error_type = FLAG_ERROR_NO_VALUE;
                            flag_context.errored_flag = arg;
                            return false;
                        }

                        char *val = next_arg(&argc, &argv);
                        flag->data.str = val;
                        break;
                    }

                    default:
                        assert(0 && "Unhandled flag type");
                        break;
                }

                // we found the matching flag so break out from the loop
                break;
            }

            found = true;
        }

        if (found == false) {
            flag_context.error_type = FLAG_ERROR_UNKNOWN;
            flag_context.errored_flag = arg;
            return false;
        }
    }

    return true;
}

void print_flag_error(FILE *out) {
    switch (flag_context.error_type) {
        case FLAG_NO_ERROR:
            break;
        case FLAG_ERROR_UNKNOWN:
            fprintf(out, "ERROR: Unrecognized flag: '%s'\n", flag_context.errored_flag);
            break;
        case FLAG_ERROR_NO_VALUE:
            fprintf(out, "ERROR: You did not provide a flag value for: '%s'\n", flag_context.errored_flag);
            break;
        case FLAG_ERROR_INVALID_VALUE:
            fprintf(out, "ERROR: Could not parse flag value for flag '%s'\n", flag_context.errored_flag);
            break;
        default:
            assert(0 && "Unhandled error type");
            break;
    }
}
