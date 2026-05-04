#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int parse_int(char* s, int* out) {
    char *end;
    errno = 0;

    long val = strtol(s, &end, 10);

    if (end == s) {
        return -1;
    }

    if (*end != '\0') {
        return -2;
    }

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))) {
        return -3;
    }

    if (val > INT_MAX || val < INT_MIN) {
        return -4;
    }

    *out = (int)val;
    return 0;
}