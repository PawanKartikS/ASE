#pragma once

/* clang-format off */
#define BUF_SZ        128
#define SEG_INIT_ADDR 0

#define TERM_RED   "\x1b[31m"
#define TERM_GREEN "\x1b[32m"
#define TERM_RESET "\x1b[0m"
/* clang-format */

#define FLAG_AF "UF"
#define FLAG_CF "EF"
#define FLAG_IF "IF"
#define FLAG_PF "PF"
#define FLAG_ZF "ZF"

#define REG_AX "AX"
#define REG_BX "BX"
#define REG_CX "CX"
#define REG_DX "DX"

#include <stdio.h>

typedef struct flags {
	int f_ch[8];
	int af, cf, df, iif, of, pf, sf, zf;
} flags_t;

typedef struct mem_nodes {
	char val[BUF_SZ];
	int seg, offset, addr;
	struct mem_nodes *next;
} mem_nodes_t;

typedef struct mem {
	int si, di;
	int ds, es, warned;
	mem_nodes_t *head;
} mem_t;

typedef struct stack {
	int top;
	char *arr[BUF_SZ];
} stack_t;

typedef struct registers {
	char ax[BUF_SZ], bx[BUF_SZ],
         cx[BUF_SZ], dx[BUF_SZ];
} registers_t;

typedef struct glob {
	int idx;
	struct label_loc {
		int line;
		char label[BUF_SZ];
	} label_locs[256];

	FILE *fd;
	int c_line,         /* Current line */
        c_len,          /* Current line length */
        p_len,          /* Previous line length */
        debug,          /* Debugging mode on? */
        opcount;        /* Operand count */
	char label[BUF_SZ], to_label[BUF_SZ], tokens[3][BUF_SZ];

	mem_t *mem;
	flags_t *flags;
	stack_t *stack;
	registers_t *registers;
} glob_t;

/* clang-format off */
mem_nodes_t *addr_node     (glob_t *glob, int addr);
void         destroy_glob  (glob_t *glob);
glob_t      *init_glob     (FILE   *fd);
int          lahf          (glob_t *glob, char *buf,
                            unsigned long size);
char        *opptr         (glob_t *glob, char *operand);
int          opval         (glob_t *glob, char *operand,
                            char *buf, unsigned long size);
int          org           (glob_t *glob, char *buf,
                            unsigned long size);
mem_nodes_t *register_addr (glob_t *glob, int seg,
                            int offset);
char        *regptr        (glob_t *glob, char *reg);
int          sahf          (glob_t *glob, char *buf,
                            unsigned long size);
/* clang-format on */