/* Unit test for instructions that PUSH and POP data from/to
 * stack. */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../Glob/glob.h"
#include "../Memory/mem.h"
#include "../Parser/parse.h"
#include "../Stack/stack.h"

int main(void) {
    FILE *fd = fopen("Tests/ph", "r");
    if (!fd) {
        fprintf(stderr, "TEST: STACK - Could not open PH.\n");
        return 1;
    }

    glob_t *glob = init_glob(fd);
    if (!glob) {
        fprintf(stderr, "TEST: STACK - Glob is NULL.\n");
        return 1;
    }

    char l_1[] = "MOV AX, 1234H";
    char l_2[] = "MOV BX, 0H";
    char l_3[] = "PUSH AX";
    char l_4[] = "PUSH BX";
    char l_5[] = "POP DX";

    parse(glob, l_1);
    move(glob, NULL, BUF_SZ);
    if (strcmp(glob->registers->ax, "1234") != 0) {
        fprintf(stderr, "TEST: STACK - Failed to set AX value.\n");
        return 1;
    }

    parse(glob, l_2);
    move(glob, NULL, BUF_SZ);
    if (strcmp(glob->registers->bx, "0") != 0) {
        fprintf(stderr, "TEST: STACK - Failed to set BX value.\n");
        return 1;
    }

    parse(glob, l_3);
    push(glob, NULL, BUF_SZ);
    if (glob->stack->top != 0 || strcmp(glob->stack->arr[0], "1234") != 0) {
        fprintf(stderr, "Test MOV: Could not push AX to stack.\n");
        return 1;
    }

    parse(glob, l_4);
    push(glob, NULL, BUF_SZ);
    if (glob->stack->top != 1 || strcmp(glob->stack->arr[1], "0") != 0) {
        fprintf(stderr, "Test MOV: Could not push BX to stack.\n");
        return 1;
    }

    parse(glob, l_5);
    pop(glob, NULL, -1);
    if (glob->stack->top != 0 || strcmp(glob->registers->dx, "0") != 0) {
        fprintf(stderr, "TEST MOV: Failed to pop to DX.\n");
        return 1;
    }

    fclose(fd);
    return 0;
}