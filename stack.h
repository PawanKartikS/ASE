#pragma once

#include "glob.h"
#include "parse.h"

/* clang-format off */
int pop  (glob_t *glob, char *buf, unsigned long size);
int push (glob_t *glob, char *buf, unsigned long size);
/* clang-format on */