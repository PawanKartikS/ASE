#ifndef _ASE_MATH_
#define _ASE_MATH_

#include "../Glob/glob.h"
#include "../Parser/parse.h"

#define ADD "ADD"
#define CMP "CMP"
#define DIV "DIV"
#define MUL "MUL"
#define SUB "SUB"

int math_op(glob_t *glob, char *buf, unsigned long size);

#endif