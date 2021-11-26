#include "flags.h"
#include "util.h"
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
    FLAG_ERROR_COUNT
} FlagErrors;

typedef union FlagData {
    int as_int;
    bool as_bool;
    char *as_str;
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
    int flag_count;

    FlagErrors error_type;
    char *errored_flag; /* The name of the flag which errored */
} FlagContext;

static FlagContext flag_global_context = {.flag_count = 0, .error_type = FLAG_NO_ERROR};

/**
 * @brief Creates a new flag in the global context
 * 
 * @param type The type of the flag
 * @param name The name of the flag
 * @param desc The description of the flag
 * @return Flag* The pointer to the newly created flag
 */
static Flag *new_flag(FlagTypes type, const char *name, const char *desc) {
    FlagContext *c = &flag_global_context;
    assert(c->flag_count < FLAG_CAP);
    Flag *flag = &(c->flags[c->flag_count++]);
    flag->type = type;
    flag->name = name;
    flag->desc = desc;
    return flag;
}

/**
 * @brief Creates a new int flag in the global context
 * 
 * @param name The name of the flag
 * @param def The default value of the flag
 * @param desc The description of the flag
 * @return int* The pointer to the flags current value
 */
int *new_int_flag(const char *name, int def, const char *desc) {
    Flag *new = new_flag(FLAG_INT, name, desc);
    new->def.as_int = def;
    new->data.as_int = def;
    return &(new->data.as_int);
}

/**
 * @brief Creates a new bool flag in the global context
 * 
 * @param name The name of the flag
 * @param def The default value of the flag
 * @param desc The description of the flag
 * @return bool* The pointer to the flags current value
 */
bool *new_bool_flag(const char *name, bool def, const char *desc) {
    Flag *new = new_flag(FLAG_BOOL, name, desc);
    new->def.as_bool = def;
    new->data.as_bool = def;
    return &(new->data.as_bool);
}

/**
 * @brief Creates a new string flag in the global context
 * 
 * @param name The name of the flag
 * @param def The default value of the flag
 * @param desc The description of the flag
 * @return char** The pointer to the flags current value
 */
char **new_str_flag(const char *name, char *def, const char *desc) {
    Flag *new = new_flag(FLAG_STR, name, desc);
    new->def.as_str = def;
    new->data.as_str = def;
    return &(new->data.as_str);
}

/**
 * @brief Prints the usage information of the flags 
 * in the global context
 * 
 * @param stream The stream to output the usage information in
 */
void print_flag_usage(FILE *stream) {
    FlagContext *c = &flag_global_context;
    fprintf(stream, "USAGE:\n");
    for (int i = 0; i < c->flag_count; i++) {
        Flag *flag = &(c->flags[i]);
        switch (flag->type) {
            case FLAG_INT:
                fprintf(stream, "    -%s <int> Default: %d\n", flag->name, flag->def.as_int);
                fprintf(stream, "        %s\n", flag->desc);
                break;

            case FLAG_BOOL:
                fprintf(stream, "    -%s Default: %s\n", flag->name, flag->def.as_bool ? "true" : "false");
                fprintf(stream, "        %s\n", flag->desc);
                break;

            case FLAG_STR: {
                fprintf(stream, "    -%s <string>", flag->name);
                if (flag->def.as_str != NULL)
                    fprintf(stream, " Default: %s", flag->def.as_str);
                else
                    fprintf(stream, "\n");

                fprintf(stream, "        %s\n", flag->desc);
                break;
            }

            default:
                assert(0 && "Unhandled flag type");
                break;
        }
        fprintf(stream, "%s", i == c->flag_count - 1 ? "" : "\n");
    }
}

/**
 * @brief It shifts the arguments at returns the current one
 * 
 * @param argc The number of arguments left
 * @param argv The array of arguments
 * @return char* The current argument
 */
static char *next_arg(int *argc, char **argv[]) {
    assert(*argc > 0);
    char *result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

/**
 * @brief Parses the arguments as specified in the global context
 * 
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return true It successfully parsed all arguments
 * @return false It encountered an error with one of the arguments
 */
bool parse_flags(int argc, char *argv[]) {
    // the first arg is always the execution so we can throw that away
    next_arg(&argc, &argv);

    FlagContext *c = &flag_global_context;

    while (argc > 0) {
        char *arg = next_arg(&argc, &argv);
        bool found = false;
        for (int i = 0; i < c->flag_count; i++) {
            Flag *flag = &(c->flags[i]);

            // parse: -flag_name
            if (1 < strlen(arg) && strcmp(arg + 1, flag->name) == 0) {

                // parse the flag
                switch (flag->type) {
                    case FLAG_INT: {
                        if (argc == 0) {
                            c->error_type = FLAG_ERROR_NO_VALUE;
                            c->errored_flag = arg;
                            return false;
                        }

                        char *val = next_arg(&argc, &argv);
                        // if sscanf did not successfully read in a value
                        if (sscanf(val, "%d", &(flag->data.as_int)) != 1) {
                            c->error_type = FLAG_ERROR_INVALID_VALUE;
                            c->errored_flag = arg;
                            return false;
                        }
                        break;
                    }

                    case FLAG_BOOL: {
                        flag->data.as_bool = true;
                        break;
                    }

                    case FLAG_STR: {
                        if (argc == 0) {
                            c->error_type = FLAG_ERROR_NO_VALUE;
                            c->errored_flag = arg;
                            return false;
                        }

                        char *val = next_arg(&argc, &argv);
                        flag->data.as_str = val;
                        break;
                    }

                    default:
                        assert(0 && "Unhandled flag type");
                        break;
                }
                found = true;
            }
        }

        if (found == false) {
            c->error_type = FLAG_ERROR_UNKNOWN;
            c->errored_flag = arg;
            return false;
        }
    }

    return true;
}

/**
 * @brief Prints the error encountered while parsing the flags 
 * 
 * @param stream The stream to output the error in
 */
void print_flag_error(FILE *stream) {
    FlagContext *c = &flag_global_context;

    switch (c->error_type) {
        case FLAG_NO_ERROR:
            fprintf(stream, "There were no errors during flag parsing\n");
            break;

        case FLAG_ERROR_UNKNOWN:
            fprintf(stream, "ERROR: Unrecognized flag: '%s'\n", c->errored_flag);
            break;

        case FLAG_ERROR_NO_VALUE:
            fprintf(stream, "ERROR: You did not provide a flag value for: '%s'\n", c->errored_flag);
            break;

        case FLAG_ERROR_INVALID_VALUE:
            fprintf(stream, "ERROR: Could not parse value for flag: '%s'\n", c->errored_flag);
            break;

        default:
            assert(0 && "Unhandled error type");
            break;
    }
}
