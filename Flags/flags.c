#include "flags.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int cmc(glob_t *glob, char *buf, unsigned long size) {
    assert(glob);
    glob->flags->cf = !glob->flags->cf;
    return 1;
}

int clear_flag(glob_t *glob, char *buf, unsigned long size) {
    assert(glob);
    const char *instr = glob->tokens[0];
    const char back = instr[strlen(instr) - 1];

    /* clang-format off */
    switch (back) {
    case 'C': glob->flags->cf  = 0; return 1;  /* CLC */
    case 'D': glob->flags->df  = 0; return 1;  /* CLD */
    case 'I': glob->flags->iif = 0; return 1;  /* CLI */
    }
    /* clang-format on */

    return 0;
}

int get_flag_val(glob_t *glob, char *flag) {
    assert(glob);
    assert(flag);

    switch (*flag) {
    case 'U': return glob->flags->af;
    case 'E': return glob->flags->cf;
    case 'I': return glob->flags->iif;
    case 'P': return glob->flags->pf;
    case 'Z': return glob->flags->zf;
    default: return -1;
    }

    return -1;
}

int set_flag(glob_t *glob, char *buf, unsigned long size) {
    assert(glob);
    const char *instr = glob->tokens[0];
    const char back = instr[strlen(instr) - 1];

    /* clang-format off */
    switch (back) {
    case 'C': glob->flags->cf  = 1; return 1;  /* STC */
    case 'D': glob->flags->df  = 1; return 1;  /* STD */
    case 'I': glob->flags->iif = 1; return 1;  /* STI */
    }
    /* clang-format on */

    return 0;
}