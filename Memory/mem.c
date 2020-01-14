#include "mem.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int hlt(glob_t *glob, char *buf, unsigned long size)
{
	return -1;
}

int move(glob_t *glob, char *buf, unsigned long size)
{
	assert(glob);
	char *dptr = glob->tokens[1];
	char *sptr = glob->tokens[2];

	/* Strict size checking */
	if (operand_reg(dptr) && operand_reg(sptr) &&
	    (register_size(dptr) != register_size(sptr))) {
		fprintf(stderr, "move(): both registers must be of "
				"same size.\n");
		return 0;
	}

	dptr = opptr(glob, dptr);
	return opval(glob, sptr, dptr, BUF_SZ);
}

int neg(glob_t *glob, char *buf, unsigned long size)
{
	assert(glob);
	char *operand = glob->tokens[1];
	char *ptr = opptr(glob, operand);

	if (!ptr) {
		fprintf(stderr, "neg(): Invalid operand [%s].\n", operand);
		return 0;
	}

	int temp = (int)strtol(ptr, NULL, 16) * -1;
	sprintf(ptr, "%x", temp);
	return 1;
}

int nop(glob_t *glob, char *buf, unsigned long size)
{
	return 1;
}

int port(glob_t *glob, char *buf, unsigned long size)
{
	fprintf(stderr, "port(): IN & OUT retained for compatibility.\n");
	return 1;
}

int unary(glob_t *glob, char *buf, unsigned long size)
{
	assert(glob);
	char *operand = glob->tokens[1];
	char *ptr = opptr(glob, operand);
	int uop = !strcmp(glob->tokens[0], "INC") ? 1 : -1;

	if (!ptr) {
		fprintf(stderr, "unary(): Invalid operand [%s].\n", operand);
		return 0;
	}

	sprintf(ptr, "%x", (int)strtol(ptr, 0, 16) + uop);
	return 1;
}

int xchg(glob_t *glob, char *buf, unsigned long size)
{
	assert(glob);
	char *dptr = glob->tokens[1];
	char *sptr = glob->tokens[2];

	if (operand_addr(sptr) && operand_addr(dptr)) {
		fprintf(stderr, "xchg(): Both the operands cannot "
				"be memory addresses.\n");
		return 0;
	}

	dptr = opptr(glob, dptr);
	sptr = opptr(glob, sptr);

	assert(dptr);
	assert(sptr);

	char temp[BUF_SZ] = { 0 };
	memcpy(temp, dptr, BUF_SZ);
	memcpy(dptr, sptr, BUF_SZ);
	memcpy(sptr, temp, BUF_SZ);

	return 1;
}
