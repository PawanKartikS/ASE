/* Unit test for instructions that parse source lines
 * [PARSE]. */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../glob.h"
#include "../mem.h"
#include "../parse.h"

int main(void) {
    FILE *fd = fopen("tests/ph", "r");
    if (!fd) {
        fprintf(stderr,
                "TEST: PARSE - Could not open PH.\n");
        return 1;
    }

    glob_t *glob = init_glob(fd);
    if (!glob) {
        fprintf(stderr, "TEST: PARSE - Glob is NULL.\n");
        return 1;
    }

    char buf[BUF_SZ];
    binary_repr(4, buf, sizeof(buf));
    if (strcmp(buf, "0000000000000100") != 0) {
        fprintf(stderr, "TEST: PARSE - Could not convert 4 "
                        "to binary repr.\n");
        return 1;
    }

    binary_repr(5, buf, sizeof(buf));
    if (strcmp(buf, "0000000000000101") != 0) {
        fprintf(stderr, "TEST: PARSE - Could not convert 4 "
                        "to binary repr.\n");
        return 1;
    }

    binary_repr(10, buf, sizeof(buf));
    if (strcmp(buf, "0000000000001010") != 0) {
        fprintf(stderr, "TEST: PARSE - Could not convert 4 "
                        "to binary repr.\n");
        return 1;
    }

    return 0;
}