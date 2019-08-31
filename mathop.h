/**
 * @file: mathop
 * @desc: Declares the following 8086 instructions:
 *        a) ADD
 *        b) SUB
 *        c) MUL
 *        d) DIV
 *        e) CMP
 */

#ifndef _ASE_MATH_
#define _ASE_MATH_

#include "glob.h"
#include "parse.h"

#define ADD "ADD"
#define CMP "CMP"
#define DIV "DIV"
#define MUL "MUL"
#define SUB "SUB"

int math_op(glob_t *glob, char *buf, unsigned long size);

#endif