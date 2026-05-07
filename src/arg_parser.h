#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_ERROR(expr) int error = expr; \
if (error != 0) { \
    return error; \
}

#define CONTEXT(expr, context) int error = expr; \
if (error != 0) { \
    context; \
    return error; \
}

#define ARG_INIT() int i = 1;

#define STR_ARG(var) if (i < argc) { \
    *var = argv[i++]; \
} else { \
    fprintf(stderr, "Error: Missing positional argument %d\n", i); \
    return -1; \
}

#define INT_ARG(var) if (i < argc) { \
    CONTEXT(parse_int(argv[i++], var), fprintf(stderr, "While parsing positional argument %d\n", i - 1)); \
} else { \
    fprintf(stderr, "Error: Missing positional argument %d\n", i); \
    return -1; \
}

#define OPT_ARGS(args) while (i < argc) { \
    args { \
        fprintf(stderr, "Error: No optional argument named %s\n", argv[i]); \
        return -1; \
    } \
}

#define OPT_STR_ARG(arg, var) if (!strcmp(argv[i], arg)) { \
    i++; \
    if (i < argc) { \
        *var = argv[i]; \
    } else { \
        fprintf(stderr, "Error: Optional argument %s requires a value\n", argv[i - 1]); \
        return -1; \
    } \
    i++; \
} else

#define OPT_INT_ARG(arg, var) if (!strcmp(argv[i], arg)) { \
    i++; \
    if (i < argc) { \
        CONTEXT(parse_int(argv[i++], var), fprintf(stderr, "While parsing value for optional argument %s\n", argv[i - 2])); \
    } else { \
        fprintf(stderr, "Error: Optional argument %s requires a value\n", argv[i - 1]); \
        return -1; \
    } \
    i++; \
} else

int parse_int(char* s, int* out);

#ifdef __cplusplus
}
#endif