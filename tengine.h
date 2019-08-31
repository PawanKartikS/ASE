#pragma once

/* clang-format off */
#define RESV    "resv"
#define BUFSIZE 128
/* clang-format on */

#include "glob.h"
#include "parse.h"

typedef struct entry {
    char *f_id;
    int opcount;
    int (*f_ptr)(glob_t *glob, char *buf, unsigned long size);
    struct entry *next;
} entry_t;

typedef struct table {
    entry_t *head;
    unsigned int entries;
} table_t;

/* clang-format off */
int      call_by_name   (table_t *table, glob_t *glob,
                         char *buf, unsigned long size);
void     destroy_table  (table_t *table);
int      entry_exists   (table_t *table, char *f_id);
table_t *init_table     (void);
int      register_entry (table_t *table,
                         char *f_id,
                         int (*f_ptr)(glob_t *glob,
                                      char *buf,
                                      unsigned long size),
                         int opcount);
/* clang-format on */
