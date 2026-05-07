#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int parse_int(char* s, int* out) {
    char *end;
    errno = 0;

    long val = strtol(s, &end, 10);

    if (end == s) {
        fprintf(stderr, "Error: Failed to parse int\n");
        return -1;
    }

    if (*end != '\0') {
        fprintf(stderr, "Error: Failed to parse int\n");
        return -1;
    }

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))) {
        fprintf(stderr, "Error: Failed to parse int\n");
        return -1;
    }

    if (val > INT_MAX || val < INT_MIN) {
        fprintf(stderr, "Error: Failed to parse int\n");
        return -1;
    }

    *out = (int)val;
    return 0;
}