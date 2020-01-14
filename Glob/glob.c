#include "glob.h"
#include "../Parser/parse.h"
#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

mem_nodes_t *addr_node(glob_t *glob, int addr)
{
	assert(glob);
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

void destroy_glob(glob_t *glob)
{
	assert(glob);
	fclose(glob->fd);

	mem_nodes_t *node = glob->mem->head;
	while (node) {
		mem_nodes_t *temp = node;
		node = node->next;
		free(temp);
	}

	free(glob->mem);
	free(glob->flags);

	int top = glob->stack->top;
	while (top >= 0) {
		free(glob->stack->arr[top--]);
	}

	free(glob->stack);
	free(glob->registers);
	free(glob);
}

glob_t *init_glob(FILE *fd)
{
	glob_t *glob = malloc(sizeof(glob_t));
	assert(fd);
	assert(glob);

	glob->mem = malloc(sizeof(mem_t));
	glob->flags = malloc(sizeof(flags_t));
	glob->stack = malloc(sizeof(stack_t));
	glob->registers = malloc(sizeof(registers_t));

	assert(glob->mem);
	assert(glob->flags);
	assert(glob->stack);
	assert(glob->registers);

	memset(glob->mem, 0, sizeof(mem_t));
	memset(glob->flags, 0, sizeof(flags_t));
	memset(glob->registers, 0, sizeof(registers_t));
	memset(glob->flags->f_ch, 0, sizeof(glob->flags->f_ch));

	glob->fd = fd;
	glob->stack->top = -1;
	return glob;
}

int lahf(glob_t *glob, char *buf, unsigned long size)
{
	assert(glob);
	assert(glob->opcount == 0);
	sprintf(glob->registers->ax, "%d%d%d%d%d", glob->flags->sf,
		glob->flags->zf, glob->flags->af, glob->flags->pf,
		glob->flags->cf);

	return 1;
}

char *opptr(glob_t *glob, char *operand)
{
	if (operand_reg(operand)) {
		return regptr(glob, operand);
	}

	char addr[BUF_SZ] = { 0 };
	memcpy(addr, &operand[1], strlen(operand) - 2);

	if (!valid_addr(addr)) {
		/* User probably supplied a literal */
		return NULL;
	}

	mem_nodes_t *node = addr_node(glob, (int)strtol(addr, NULL, 0));
	return (node) ? node->val :
			register_addr(glob, SEG_INIT_ADDR,
				      (int)strtol(addr, NULL, 0))
				->val;
}

int opval(glob_t *glob, char *operand, char *buf, unsigned long size)
{
	assert(glob);

	/**
     * In some occasions, we set size to -1. This is a hack,
     * to indicate that value of operand must be copied to
     * buf as it is, without any mod- -ifications. Eg:
     * popping from stack (No conversions, direct copy)
     *
     * In such cases, we acknowledge the request, and set
     * the size back to BUF_SZ.
     */
	if (size == -1) {
		/* ab_lit - absolute hex literal (no modifications)
         */
		goto ab_lit;
	}

	if (operand_addr(operand)) {
		char addr[BUF_SZ] = { 0 };
		memcpy(addr, &operand[1], strlen(operand) - 2);
		assert(valid_addr(addr));

		mem_nodes_t *node = addr_node(glob, (int)strtol(addr, NULL, 0));
		if (node) {
			memcpy(buf, node->val, BUF_SZ);
			return 1;
		}

		return 0;
	}

	if (operand_reg(operand)) {
		char *ptr = regptr(glob, operand);
		memcpy(buf, ptr, size);
		return 1;
	}

ab_lit:;
	/* Operand is a literal */
	char *lchar = &operand[strlen(operand) - 1];
	if (*lchar == 'H' || *lchar == 'h' || size == -1) {
		if (size != -1) {
			*lchar = '\0';
		} else {
			size = (unsigned long)BUF_SZ;
		}

		/* Validate hex */
		if (!valid_hex(operand)) {
			fprintf(stderr, "opval(): Invalid hex literal.\n");
			return 0;
		}

		memcpy(buf, operand, size);
		return 1;
	} else {
		/* Need to convert the literal to hex */
		const int ksz = strlen(operand);
		for (int i = 0; i < ksz; i++) {
			if (!isdigit(operand[i]) && operand[i] != '-') {
				fprintf(stderr, "Invalid str literal [%s].\n",
					operand);
				return 0;
			}
		}

		char temp[BUF_SZ] = { 0 };
		int val = (int)strtol(operand, NULL, 0);

		/* Set flag values */
		glob->flags->pf = __builtin_popcount(val) % 2 == 0 && val != 0;
		glob->flags->zf = val == 0;

		/* Check for overflow */
		if (val > 32767 || val < -32768) {
			fprintf(stderr,
				"opval(): Operand value too large [%s].\n",
				operand);
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

int org(glob_t *glob, char *buf, unsigned long size)
{
	assert(glob);

	/**
     * There is nothing to do with ORG's operand. Retained
     * for compatibility.
     */
	char *operand = glob->tokens[1];
	int max = strlen(operand);

	if (operand[max - 1] != 'h' && operand[max - 1] != 'H') {
		fprintf(stderr, "org(): Required hex suffix.\n");
		return 0;
	}

	operand[max - 1] = '\0';
	if (!valid_hex(operand)) {
		fprintf(stderr, "org(): Invalid address [%s].\n", operand);
		return 0;
	}

	return 1;
}

mem_nodes_t *register_addr(glob_t *glob, int seg, int offset)
{
	assert(glob);
	mem_nodes_t *node = malloc(sizeof(mem_nodes_t));
	if (!node) {
		fprintf(stderr, "register_addr(): malloc failure.\n");
		return NULL;
	}

	node->next = NULL;
	node->seg = seg;
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

char *regptr(glob_t *glob, char *reg)
{
	assert(glob);
	assert(reg);

	char *ptr = NULL;

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
	case 0:
		ptr = glob->registers->ax;
		break;
	case 1:
		ptr = glob->registers->bx;
		break;
	case 2:
		ptr = glob->registers->cx;
		break;
	case 3:
		ptr = glob->registers->dx;
		break;
	default:
		return NULL;
	}

	if (!ptr) {
		ptr = malloc(BUF_SZ);
	}

	return ptr;
}

int sahf(glob_t *glob, char *buf, unsigned long size)
{
	assert(glob);
	assert(glob->opcount == 0);
	glob->flags->sf = glob->registers->ax[0] == '1';
	glob->flags->zf = glob->registers->ax[1] == '1';
	glob->flags->af = glob->registers->ax[2] == '1';
	glob->flags->pf = glob->registers->ax[3] == '1';
	glob->flags->cf = glob->registers->ax[4] == '1';

	return 1;
}