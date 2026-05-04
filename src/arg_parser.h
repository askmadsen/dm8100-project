#include <stdlib.h>
#include <string.h>

#define CHECK_ERROR(expr) int error = expr; \
if (error != 0) { \
    return error; \
}

#define ARG_INIT() int i = 1;

#define STR_ARG(var) if (argc >= i) { \
    *var = argv[i++]; \
} else { \
    return -1; \
}

#define INT_ARG(var) if (argc >= i) { \
    return parse_int(argv[i++], var); \
} else { \
    return -1; \
}

#define OPT_ARGS() while (i < argc)
#define OPT_ARGS2(args) while (i < argc) { \
    args { return -1; } \
}
#define OPT_END() { return -1; }

#define OPT_STR_ARG(arg, var) if (!strcmp(argv[i], arg)) { \
    i++; \
    if (i < argc) { \
        *var = argv[i]; \
    } \
    i++; \
} else

#define OPT_INT_ARG(arg, var) if (!strcmp(argv[i], arg)) { \
    i++; \
    if (i < argc) { \
        CHECK_ERROR(parse_int(argv[i++], var)); \
    } \
    i++; \
} else

int parse_int(char* s, int* out);