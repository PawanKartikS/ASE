#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mathop.h"

int math_op(glob_t *glob, char *buf, unsigned long size) {
    if (!glob) {
        fprintf(stderr, "math_op(): glob - null\n");
        return 0;
    }

    int ret = 1;
    /* Let AX (accumulator) be the default destination */
    char *ptr = regptr(glob, REG_AX);

    char *inst = glob->tokens[0];
    char *dptr = glob->tokens[1];
    char *sptr = glob->tokens[2];
    char dval[BUF_SZ], sval[BUF_SZ], res[BUF_SZ];

    if (strcmp(inst, DIV) == 0 || strcmp(inst, MUL) == 0) {
        /**
         * DIV & MUL take only 1 operand.
         * The default and the destination operand is AX
         * (accumulator).
         */
        dptr = REG_AX;
        sptr = glob->tokens[1];
    }

    int ret1 = opval(glob, dptr, dval, sizeof(dval));
    int ret2 = opval(glob, sptr, sval, sizeof(sval));

    if (ret1 ^ ret2) {
        return 0;
    }

    int c_dval = (int)strtol(dval, NULL, 16);
    int c_sval = (int)strtol(sval, NULL, 16);

    c_dval = (int16_t)c_dval;
    c_sval = (int16_t)c_sval;

    if (strcmp(inst, ADD) == 0) {
        int ans = c_dval + c_sval;
        glob->flags->zf = ans == 0;

        if (ans > 32767 || ans < -32768) {
            strcpy(res, "0");
            glob->flags->of = 1;
            fprintf(stderr, "Overflow: operand value "
                            "exceeds the limits.\n");
            ret = 0;
        } else {
            sprintf(res, "%d", ans);
        }
    } else if (strcmp(inst, SUB) == 0) {
        /* Answer is 0. Set zero flag */
        if (c_dval == c_sval) {
            glob->flags->zf = 1;
        }

        sprintf(res, "%d", c_dval - c_sval);
    } else if (strcmp(inst, MUL) == 0) {
        sprintf(res, "%d", c_dval * c_sval);
        goto set;
    } else if (strcmp(inst, DIV) == 0) {
        sprintf(res, "%d", c_dval / c_sval);
        goto set;
    } else if (strcmp(inst, CMP) == 0) {
        if (c_dval < c_sval) {
            glob->flags->cf = 1;
        } else if (c_dval == c_sval) {
            glob->flags->zf = 1;
        }
        return 1;
    } else {
        return 0;
    }

    /**
     * Set the destination pointer.
     * This part is skipped if the default destination has
     * been set to AX (accumulator).
     */
    if (operand_addr(dptr)) {
        char addr[BUF_SZ];
        memset(addr, 0, sizeof(addr));
        memcpy(addr, &dptr[1], strlen(dptr) - 2);

        if (!valid_addr(addr)) {
            fprintf(stderr, "move(): invalid address [%s].\n", addr);
            return 0;
        }

        int offset = (int)strtol(addr, NULL, 0);
        ptr = register_addr(glob, 0, offset)->val;
    } else if (operand_reg(dptr)) {
        ptr = regptr(glob, dptr);
    }

/**
 * We skip to this part if AX (accumulator) has been decided
 * as the default destination.
 */
set:
    assert(ptr);
    return ret & opval(glob, res, ptr, BUF_SZ);
}