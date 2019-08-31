#include "tengine.h"
#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

int call_by_name(table_t *table, glob_t *glob, char *buf, unsigned long size) {
    int x = 0;
    const int klen = strlen(glob->tokens[0]);
    char *f_id = glob->tokens[0], *ptr = f_id;

    while (*ptr) {
        if (isspace(*ptr++))
            x++;
    }

    if (x == klen) {
        return 1;
    }

    entry_t *entry = table->head;
    while (entry) {
        if (strcmp(entry->f_id, f_id)) {
            entry = entry->next;
            continue;
        }

        if (entry->opcount != glob->opcount) {
            fprintf(stderr, "Mismatch operand count.");
            return 0;
        }

        return entry->f_ptr(glob, buf, size);
    }

    fprintf(stderr, "Instruction [%s] not supported.\n", ptr);
    return 0;
}

void destroy_table(table_t *table) {
    if (!table) {
        fprintf(stderr, "Invalid table - points to NULL.\n");
        return;
    }

    entry_t *entry = table->head;
    while (entry) {
        free(entry->f_id);
        entry_t *x = entry;
        entry = entry->next;
        free(x);
    }

    free(table);
}

int entry_exists(table_t *table, char *f_id) {
    if (!table) {
        return 0;
    }

    entry_t *entry = table->head;
    while (entry) {
        if (!strcmp(entry->f_id, f_id)) {
            return 1;
        }

        entry = entry->next;
    }

    return 0;
}

table_t *init_table(void) {
    table_t *table = malloc(sizeof(table_t));
    return table;
}

int register_entry(table_t *table, char *f_id,
                   int (*f_ptr)(glob_t *glob, char *buf, unsigned long size),
                   int opcount) {
    assert(table);
    entry_t *entry = malloc(sizeof(entry_t));
    if (entry) {
        entry->opcount = opcount;
        entry->f_id = malloc(BUFSIZE);
        memcpy(entry->f_id, f_id, BUFSIZE);
        entry->f_ptr = f_ptr;
        table->entries++;

        entry->next = table->head;
        table->head = entry;

        return 1;
    } else {
        fprintf(stderr, "malloc() fail: register_entry");
    }

    return 0;
}
