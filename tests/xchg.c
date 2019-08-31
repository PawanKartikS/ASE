/* Unit test for instructions that swap data [XCHG]. */

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
                "TEST: XCHG - Could not open PH.\n");
        return 1;
    }

    glob_t *glob = init_glob(fd);
    if (!glob) {
        fprintf(stderr, "TEST: XCHG - Glob is NULL.\n");
        return 1;
    }

    char l_1[] = "MOV [1128], 10";
    char l_2[] = "MOV BX, 11";
    char l_3[] = "MOV AX, 12";
    char l_4[] = "XCHG [1128], BX";
    char l_5[] = "XCHG AX, BX";

    parse(glob, l_1);
    move(glob, NULL, BUF_SZ);

    parse(glob, l_2);
    move(glob, NULL, BUF_SZ);

    parse(glob, l_3);
    move(glob, NULL, BUF_SZ);

    parse(glob, l_4);
    xchg(glob, NULL, BUF_SZ);

    parse(glob, l_5);
    xchg(glob, NULL, BUF_SZ);

    if ((strcmp(glob->mem->head->val, "b") != 0)) {
        fprintf(stderr,
                "Test XCHG: [1128] value not modified.\n");
        return 1;
    }

    if ((strcmp(glob->registers->ax, "a") != 0)) {
        fprintf(stderr,
                "Test XCHG: [AX] value not modified.\n");
        return 1;
    }

    if ((strcmp(glob->registers->bx, "c") != 0)) {
        fprintf(stderr,
                "Test XCHG: [BX] value not modified.\n");
        return 1;
    }

    fclose(fd);
    return 0;
}