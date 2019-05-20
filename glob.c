/**
 * @file: glob.c
 * @desc: Declares structures and functions that hold register and
 *        stack data.
 */

#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "glob.h"
#include "parse.h"

/**
 * @desc  : Allows user to move data to specified mem addr.
 * @param : glob   -
 *          seg    - segment address.
 *          offset - offset address.
 *          PA     - (seg * 10) + offset
 * @return: mem_nodes_t
 */
mem_nodes_t* add_to_mem(glob_t *glob, int seg, int offset) {
	if (!glob) {
		fprintf(stderr, "add_to_mem(): glob - null ptr.\n");
		return NULL;
	}

	mem_nodes_t *node = malloc(sizeof(mem_nodes_t));
	if (!node) {
		fprintf(stderr, "add_to_mem(): malloc failure.\n");
		return NULL;
	}

	if ((!glob->mem->ds || !glob->mem->es) && !glob->mem->warned && !getenv("DIW")) {
		fprintf(stderr, "add_to_mem(): Did not init [D/E]S?\n");
		glob->mem->warned = 1;
	}

	node->next = NULL;
	node->seg  = seg;
	node->offset = offset;

	node->addr = (seg * 10) + offset;
	if (!glob->mem->head) {
		glob->mem->head = node;
		return node;
	}

	mem_nodes_t *curr = glob->mem->head, *prev = NULL;
	while (curr) {
		if (node->addr >= curr->addr) {
			prev = curr;
			curr = curr->next;
		}

		node->next = curr;
		/* Appending to the beginning of the list? */
		if (!prev) {
			glob->mem->head = node;
			return node;
		}

		prev->next = node;
		return node;
	}

	return NULL;
}

/**
 * @desc  : Destory parent structure.
 * @param : glob -
 * @return: void
 */ 
void destroy_glob(glob_t *glob) {
	if (!glob) {
		fprintf(stderr, "Invalid glob - points to NULL.\n");
		return;
	}

	fclose(glob->fd);

	mem_nodes_t *node = glob->mem->head;
	while (node) {
		mem_nodes_t *temp = node;
		node = node->next;
		free(temp);
	}

	free(glob->mem);
	free(glob->flags);

	char **ptr = &glob->stack->arr[0];
	while (*ptr) {
		free(*ptr++);
	}

	free(glob->stack);
	free(glob->registers);
	free(glob);
}

/**
 * @desc  : Returns the memory value pointed by addr.
 * @param : glob -
 *          addr - address of the value.
 *          buf  - buffer that receives the data.
 *          size - size of the buffer.
 * @return: int  - 0 if fail, 1 if success.
 */ 
mem_nodes_t* get_mem_node(glob_t *glob, int addr) {
	if (!glob) {
		fprintf(stderr, "get_mem_node(): glob - nullptr.\n");
		return 0;
	}

	mem_nodes_t *node = glob->mem->head;
	while (node) {
		int pa = (node->seg * 10) + node->offset;
		if (pa == addr) {
			return node;
		}

		node = node->next;
	}

	return NULL;
}

/**
 * @desc  : Gets the specified operand's value.
 * @param : glob -
 *          op   - operand
 *          buf  - buffer that receives the value.
 *          size - size of the buffer.
 * @return: int  - 0 if fail, 1 if success.
 */ 
int get_op_val(glob_t *glob, char *op, char *buf, unsigned long size) {
	if (!glob) {
		fprintf(stderr, "get_op_val(): glob - nullptr.\n");
		return 0;
	}

	/**
	 * In some occasions, we set size to -1. This is a hack, to indicate
	 * that value of op must be copied to buf as it is, without any mod-
	 * -ifications.
	 * 	Eg: popping from stack (No conversions, direct copy)
	 * 
	 * In such cases, we acknowledge the request, and set the size back
	 * to BUF_SZ.
	 */
	if (size == -1) {
		/* ab_lit - absolute hex literal (no modifications) */
		goto ab_lit;
	}

	if (is_op_addr(op)) {
		char addr[BUF_SZ];
		memset(addr, 0, sizeof(addr));
		memcpy(addr, &op[1], strlen(op) - 2);

		mem_nodes_t *node = get_mem_node(glob, (int)strtol(addr, NULL, 0));
		if (node) {
			memcpy(buf, node->val, BUF_SZ);
			return 1;
		}

		return 0;
	}

	if (is_op_reg(op)) {
		char *ptr = get_reg_ptr(glob, op);
		memcpy(buf, ptr, size);
		return 1;
	}

	ab_lit: ;
	/* Operand is a literal */
	char *lchar = &op[strlen(op) - 1];
	if (*lchar == 'H' || *lchar == 'h' || size == -1) {
		if (size != -1) {
			*lchar = '\0';
		} else {
			size = (unsigned long)BUF_SZ;
		}

		/* Validate hex */
		if (!is_valid_hex(op)) {
			fprintf(stderr, "get_op_val(): Invalid hex literal.\n");
			return 0;
		}

		memcpy(buf, op, size);
		return 1;
	} else {
		/* Need to convert the literal to hex */
		const int ksz = strlen(op);
		for (int i = 0; i < ksz; i++) {
			if (!isdigit(op[i]) && op[i] != '-') {
				fprintf(stderr, "get_op_val(): Invalid str literal [%s].\n", op);
				return 0;
			}
		}

		char temp[BUF_SZ];
		memset(temp, 0, sizeof(temp));
		int val = (int)strtol(op, NULL, 0);


		/* Set flag values */
		glob->flags->pf = __builtin_popcount(val) % 2 == 0 && val != 0;
		glob->flags->zf = val == 0;

		/* Check for overflow */
		if (val > 32767 || val < -32768) {
			fprintf(stderr, "get_op_val(): Operand value too large [%s].\n", op);
			glob->flags->of = 1;
			return 1;
		}

		sprintf(temp, "%x", val);
		char *beg = temp;

		if (strlen(temp) > 4) {
			beg = &temp[strlen(temp) - 1];
			beg -= 3;
		}
		
		memcpy(buf, beg, BUF_SZ);
		return 1;
	}

	return 0;
}

/**
 * @desc  : Returns the pointer to the specified register.
 * @param : glob -
 *          reg  - register's pointer that is required.
 * @return: char*
 */ 
char *get_reg_ptr(glob_t *glob, char *reg) {
	if (!glob || !reg) {
		fprintf(stderr, "get_reg_ptr(): nullptr received.\n");
		return NULL;
	}

	char *ptr  = NULL;

	/* Convert 8 bit register to 16 bit. */
	char *back = &reg[strlen(reg) - 1];
	if (*back == 'H' || *back == 'L') {
		*back = 'X';
	} else if (*back != 'X') {
		/* back - neither H, L nor X. */
		return NULL;
	}

	int diff = abs(strcmp(reg, REG_AX));
	switch (diff) {
	case 0: ptr = glob->registers->ax; break;
	case 1: ptr = glob->registers->bx; break;
	case 2: ptr = glob->registers->cx; break;
	case 3: ptr = glob->registers->dx; break;

	default: return NULL;
	}

	if (!ptr) {
		ptr = malloc(BUF_SZ);
	}

	return ptr;
}

/**
 * @desc  : Init parent and child structures.
 * @param : fd - file descriptor of source file.
 * @return: void
 */
glob_t *init_glob(FILE *fd) {
	if (!fd) {
		fprintf(stderr, "Invalid fd - points to null.\n");
		return NULL;
	}

	glob_t *glob = malloc(sizeof(glob_t));
	if (!glob) {
		return NULL;
	}
	
	glob->mem = malloc(sizeof(mem_t));
	glob->flags = malloc(sizeof(flags_t));
	glob->stack = malloc(sizeof(stack_t));
	glob->registers = malloc(sizeof(registers_t));

	assert(glob->mem);
	assert(glob->flags);
	assert(glob->stack);
	assert(glob->registers);

	glob->fd = fd;
	glob->bpnt = glob->stack->top = -1;
	glob->c_line = glob->p_len = 0;

	glob->mem->ds = glob->mem->es = 0;
	memset(glob->flags, 0, sizeof(flags_t));
	return glob;
}

/**
 * @desc  : Implements the LAHF instruction.
 * @param : glob -
 *          buf  - unused
 *          size - unused
 * @return: int  - 0 if fail, 1 if success.
 */
int lahf(glob_t *glob, char *buf, unsigned long size) {
	if (!glob) {
		fprintf(stderr, "lahf(): glob - nullptr.\n");
		return 0;
	}

	assert(glob->n_op == 0);
	sprintf(glob->registers->ax, "%d%d%d%d%d",
		glob->flags->sf,
		glob->flags->zf,
		glob->flags->af,
		glob->flags->pf,
		glob->flags->cf);

	return 1;
}

/**
 * @desc  : Implements the SAHF instruction.
 * @param : glob -
 *          buf  - unused
 *          size - unused
 * @return: int  - 0 if fail, 1 if success.
 */
int sahf(glob_t *glob, char *buf, unsigned long size) {
	if (!glob) {
		fprintf(stderr, "sahf(): glob - nullptr.\n");
		return 0;
	}

	assert(glob->n_op == 0);
	glob->flags->sf = glob->registers->ax[0] == '1';
	glob->flags->zf = glob->registers->ax[1] == '1';
	glob->flags->af = glob->registers->ax[2] == '1';
	glob->flags->pf = glob->registers->ax[3] == '1';
	glob->flags->cf = glob->registers->ax[4] == '1';

	return 1;
}