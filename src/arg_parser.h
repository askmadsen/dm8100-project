#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Propagates an error value.
#define CHECK_ERROR(expr) int error = expr; \
if (error != 0) { \
    return error; \
}

// Propagates an error value, and gives context about the error.
#define CONTEXT(expr, context) int error = expr; \
if (error != 0) { \
    context; \
    return error; \
}

// Initializes a CLI parser.
#define ARG_INIT() int i = 1;

// Add a positional string argument to the parser
#define STR_ARG(var) if (i < argc) { \
    *var = argv[i++]; \
} else { \
    fprintf(stderr, "Error: Missing positional argument %d\n", i); \
    return -1; \
}

// Add a positional int argument to the parser
#define INT_ARG(var) if (i < argc) { \
    CONTEXT(parse_int(argv[i++], var), fprintf(stderr, "While parsing positional argument %d\n", i - 1)); \
} else { \
    fprintf(stderr, "Error: Missing positional argument %d\n", i); \
    return -1; \
}

// Configures the parser to accept optional arguments.
#define OPT_ARGS(args) while (i < argc) { \
    args { \
        fprintf(stderr, "Error: No optional argument named %s\n", argv[i]); \
        return -1; \
    } \
}

// Adds an optional string argument to the parser.
#define OPT_STR_ARG(arg, var) if (!strcmp(argv[i], arg)) { \
    i++; \
    if (i < argc) { \
        *var = argv[i++]; \
    } else { \
        fprintf(stderr, "Error: Optional argument %s requires a value\n", argv[i - 1]); \
        return -1; \
    } \
} else

// Adds an optional integer argument to the parser.
#define OPT_INT_ARG(arg, var) if (!strcmp(argv[i], arg)) { \
    i++; \
    if (i < argc) { \
        CONTEXT(parse_int(argv[i++], var), fprintf(stderr, "While parsing value for optional argument %s\n", argv[i - 2])); \
    } else { \
        fprintf(stderr, "Error: Optional argument %s requires a value\n", argv[i - 1]); \
        return -1; \
    } \
} else

// Tries to parse an integer.
// The parsed integer is written to out.
// the return value is 0 for success and -1 for fail.
int parse_int(char* s, int* out);

#ifdef __cplusplus
}
#endif