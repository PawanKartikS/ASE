#include "parse.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

void binary_repr(int x, char *buf, unsigned long size)
{
	memset(buf, 0, size);
	for (int i = 15; i >= 0; i--) {
		buf[16 - i - 1] = x & 1 << i ? '1' : '0';
	}
}

int jump(glob_t *glob, char *buf, unsigned long size)
{
	if (buf && !strcmp(buf, REG_CX)) {
		char val[BUF_SZ] = { 0 };
		char *ptr = regptr(glob, REG_CX);
		memcpy(val, ptr, sizeof(val));

		if (strcmp(val, "0") != 0) {
			/* JCXZ condition failed. */
			return 1;
		} else {
			/* Allow jump execution. */
			goto jmp;
		}
	}

	if (buf && ((size == -1 && glob->flags->cf == 0) ||
		    (size != -1 && glob->flags->cf == 1) ||
		    (size == -1 && glob->flags->pf == 0) ||
		    (size != -1 && glob->flags->pf == 1) ||
		    (size == -1 && glob->flags->zf == 0) ||
		    (size != -1 && glob->flags->zf == 1))) {
		/* Required jump conditions satisfied. */
		goto jmp;
	} else {
		/* Condition failed */
		if (buf)
			return 1;
	}

jmp:;
	/* Jump */
	char line[BUF_SZ], to_label[BUF_SZ];
	memcpy(to_label, glob->tokens[1], sizeof(to_label));

	int ln = -1;
	for (int i = 0; i < glob->idx; i++) {
		if (!strcmp(glob->label_locs[i].label, to_label)) {
			ln = glob->label_locs[i].line;
		}
	}

	if (ln != -1) {
		while (glob->c_line != ln) {
			step_back(glob);
			char line[1024] = { 0 };
			fgets(line, sizeof(line), glob->fd);
		}

		assert(parse(glob, line));
		return 1;
	}

	while (fgets(line, sizeof(line), glob->fd) != NULL) {
		if (should_skip_ln(line)) {
			continue;
		}

		/* Lets hope that the parser is good enough. */
		if (!parse(glob, line)) {
			fprintf(stderr, "Could not parse line.\n");
			return 0;
		}

		int diff = strcmp(glob->label, to_label);
		if (diff == 0) {
			fseek(glob->fd, -(glob->c_len), SEEK_CUR);
			return 1;
		}
	}

	return 0;
}

int jump_cx(glob_t *glob, char *buf, unsigned long size)
{
	return jump(glob, REG_CX, size);
}

int jump_jx(glob_t *glob, char *buf, unsigned long size)
{
	const char *instr = glob->tokens[0];
	const char back = instr[strlen(instr) - 1];

	switch (back) {
	case 'C':
		return jump(glob, FLAG_CF, size); /* JC */
	case 'E':
		return jump(glob, FLAG_ZF, size); /* JE */
	case 'P':
		return jump(glob, FLAG_PF, size); /* JP */
	}

	return 0;
}

int jump_jnx(glob_t *glob, char *buf, unsigned long size)
{
	const char *instr = glob->tokens[0];
	const char back = instr[strlen(instr) - 1];

	switch (back) {
	case 'C':
		return jump(glob, FLAG_CF, (unsigned long)-1); /* JNC */
	case 'E':
		return jump(glob, FLAG_ZF, (unsigned long)-1); /* JNE */
	case 'P':
		return jump(glob, FLAG_PF, (unsigned long)-1); /* JPE */
	}

	return 0;
}

int operand_addr(char *operand)
{
	const int ksz = strlen(operand);
	/* Address must begin and end with square brackets */
	int ret = (operand[0] == '[' && operand[ksz - 1] == ']') ? 1 : 0;

	for (int i = 1; i < ksz - 1; i++) {
		if (!isdigit(operand[i])) {
			ret = 0;
		}
	}

	return ret;
}

int operand_reg(char *operand)
{
	if (strlen(operand) != 2) {
		return 0;
	}

	if (operand[0] >= 'A' && operand[0] <= 'D') {
		return (operand[1] == 'H' || operand[1] == 'L' ||
			operand[1] == 'X');
	}

	return 0;
}

int parse(glob_t *glob, char *line)
{
	assert(glob && line);
	for (int i = 0; i < 3; i++) {
		assert(glob->tokens[i]);
	}

	memset(glob->label, 0, BUF_SZ);
	for (int i = 0; i <= 2; i++) {
		memset(glob->tokens[i], 0, sizeof(glob->tokens[i]));
	}

	glob->c_line++;
	glob->p_len = glob->c_len;
	glob->c_len = strlen(line);

	int i = 0;
	int flag = 0;
	char *ptr = strtok(line, " ");

	while (ptr) {
		/* Skip if comment line begins. */
		if (*ptr == ';') {
			break;
		}

		/* There's a space between label and colon? */
		if (*ptr == ':') {
			fprintf(stderr,
				"Valid label syntax: Label: [instr] "
				"[operands] @ [%d].\n",
				glob->c_line);
			return 0;
		}

		/* There's no instruction that takes more than 2
         * operands. */
		if (i > 2) {
			fprintf(stderr, "Exceeded token limit.\n");
			return 0;
		}

		if (i == 0) {
			char *x = ptr;
			while (*x) {
				*x = toupper(*x);
				x++;
			}
		}

		if (*ptr == ',') {
			/* Are we expecting a comma? Are we parsing op2?
             */
			if (flag && i > 1) {
				flag = 0;
				goto l1;
			} else {
				fprintf(stderr, "Unexpected character [,].\n");
				return 0;
			}
		}

		/* Token can begin only with alpha-numeric chars or
         * a '['. */
		if (!isalnum(*ptr) && *ptr != '[' && *ptr != '-') {
			fprintf(stderr, "Unexpected character [%c].\n", *ptr);
			return 0;
		}

		/**
         * Check if comment line begins. No space between op
         * and ';' perhaps? Eg: instr op1 op2; This is a
         * comment line.
         */
		char *end = strchr(ptr, ';');
		if (end) {
			/* Clip off the end and return */
			*end = '\0';
			memcpy(glob->tokens[i], ptr, BUF_SZ);
			i++;
			break;
		}

		char *back = &ptr[strlen(ptr) - 1];
		/* Check for label. */
		if (*back == ':') {
			*back = '\0';
			memcpy(glob->label, ptr, BUF_SZ);
			memcpy(glob->label_locs[glob->idx].label, ptr, BUF_SZ);
			glob->label_locs[glob->idx++].line = glob->c_line;

			/* Do not count label as a token. */
			goto l1;
		}

		if (!isalnum(*back) && *back != ']') {
			*back = '\0';
		} else {
			/* We're expecting a comma. */
			flag = 1;
		}

		memcpy(glob->tokens[i], ptr, BUF_SZ);
		i++;

	l1:
		ptr = strtok(NULL, " ");
	}

	glob->opcount = i - 1;
	return 1;
}

int register_size(char *reg)
{
	assert(reg);
	if (!operand_reg(reg)) {
		fprintf(stderr, "Reg [%s] is not a valid reg.\n", reg);
		return 0;
	}

	switch (reg[1]) {
	case 'H': /* Upper 8 bit register. */
	case 'L':
		return 8; /* Lower 8 bit register. */
	case 'X':
		return 16; /* 16 bit register.      */
	}

	return 0;
}

int should_skip_ln(char *line)
{
	return (!line || line[0] == ';' || line[0] == '\n');
}

int step_back(glob_t *glob)
{
	assert(glob);
	assert(glob->fd);

	glob->c_line--;
	int new_offset = -(glob->c_len + glob->p_len);
	fseek(glob->fd, new_offset, SEEK_CUR);
	return fseek(glob->fd, -(glob->c_len), SEEK_CUR) == 0;
}

int valid_addr(char *addr)
{
	char *ptr = addr;
	while (*ptr) {
		if (!isdigit(*ptr)) {
			return 0;
		}

		ptr++;
	}

	return 1;
}

int valid_hex(char *hex)
{
	char *ptr = hex;
	while (*ptr) {
		if ((!isalpha(*ptr) && !isdigit(*ptr)) ||
		    (isalpha(*ptr) && toupper(*ptr) > 'F')) {
			return 0;
		}

		ptr++;
	}

	return 1;
}

int valid_operand(char *operand)
{
	return valid_hex(operand) || operand_addr(operand) ||
	       operand_reg(operand);
}
