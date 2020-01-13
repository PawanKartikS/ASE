#pragma once

#include "../Glob/glob.h"

/* clang-format off */
int cmc          (glob_t *glob, char *buf,
                  unsigned long size);
int clear_flag   (glob_t *glob, char *buf,
                  unsigned long size);
int get_flag_val (glob_t *glob, char *flag);
int set_flag     (glob_t *glob, char *buf,
                  unsigned long size);
/* clang-format on */