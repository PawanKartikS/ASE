#pragma once

#define BIN_FS 'B'
#define HEX_FS 'H'

#include "glob.h"
#include <stdio.h>

/* clang-format off */
void binary_repr    (int x, char *buf, unsigned long size);
int  jump           (glob_t *glob, char *buf,
                     unsigned long size);
int  jump_cx        (glob_t *glob, char *buf,
                     unsigned long size);
int  jump_jx        (glob_t *glob, char *buf,
                     unsigned long size);
int  jump_jnx       (glob_t *glob, char *buf,
                     unsigned long size);
int  operand_addr   (char *operand);
int  operand_reg    (char *operand);
int  parse          (glob_t *glob, char *line);
int  register_size  (char *reg);
int  should_skip_ln (char *line);
int  step_back      (glob_t *glob);
int  valid_addr     (char *addr);
int  valid_hex      (char *hex);
int  valid_operand  (char *operand);
/* clang-format on */