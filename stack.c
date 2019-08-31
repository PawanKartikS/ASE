#include "stack.h"
#include <assert.h>
#include <malloc.h>

int pop(glob_t *glob, char *buf, unsigned long size) {
    assert(glob);
    char *operand = glob->tokens[1];
    char *dest = opptr(glob, operand);

    assert(dest);
    int idx = glob->stack->top--;
    if (idx == -1) {
        fprintf(stderr, "Nothing to pop. Stack empty.");
        return 0;
    }

    char *sptr = glob->stack->arr[idx];
    int ret = opval(glob, sptr, dest, -1);
    free(glob->stack->arr[idx]);

    return ret;
}

int push(glob_t *glob, char *buf, unsigned long size) {
    assert(glob);
    int idx = ++glob->stack->top;
    char *operand = glob->tokens[1];
    char **sptr = &glob->stack->arr[idx];

    if (!*sptr) {
        *sptr = malloc((unsigned long)BUF_SZ);
    }

    return opval(glob, operand, *sptr, size);
}