#pragma once

#include "glob.h"
#include "parse.h"

/* clang-format off */
int hlt  (glob_t *glob, char *buf, unsigned long size);
int move (glob_t *glob, char *buf, unsigned long size);
int neg  (glob_t *glob, char *buf, unsigned long size);
int nop  (glob_t *glob, char *buf, unsigned long size);
int port (glob_t *glob, char *buf, unsigned long size);
int unary(glob_t *glob, char *buf, unsigned long size);
int xchg (glob_t *glob, char *buf, unsigned long size);
/* clang-format on */
