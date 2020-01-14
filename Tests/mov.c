/* Unit test for instructions that move data [MOV]. */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../Glob/glob.h"
#include "../Memory/mem.h"
#include "../Parser/parse.h"

int main(void)
{
	FILE *fd = fopen("Tests/ph", "r");
	if (!fd) {
		fprintf(stderr, "TEST: MOV - Could not open PH.\n");
		return 1;
	}

	glob_t *glob = init_glob(fd);
	if (!glob) {
		fprintf(stderr, "TEST: MOV - Glob is NULL.\n");
		return 1;
	}

	char l_1[] = "MOV AX, 1234H";
	char l_2[] = "MOV BX, AX";
	char l_3[] = "MOV CX, DX";
	char l_4[] = "MOV DL, 34H";
	char l_5[] = "MOV [1234], 1234";
	char l_6[] = "MOV [12], 39H";
	char l_7[] = "MOV [123], BX";

	parse(glob, l_1);
	move(glob, NULL, BUF_SZ);
	if (strcmp(glob->registers->ax, "1234") != 0) {
		fprintf(stderr, "Test: MOV - Failed to set AX value.\n");
		return 1;
	}

	parse(glob, l_2);
	move(glob, NULL, BUF_SZ);
	if (strcmp(glob->registers->bx, "1234") != 0) {
		fprintf(stderr, "Test MOV: Failed to set BX value.\n");
		return 1;
	}

	parse(glob, l_3);
	move(glob, NULL, BUF_SZ);
	if (strcmp(glob->registers->cx, "") != 0) {
		fprintf(stderr, "TEST MOV: Failed to set DX value.\n");
		return 1;
	}

	parse(glob, l_4);
	move(glob, NULL, BUF_SZ);
	if (strcmp(glob->registers->dx, "34") != 0) {
		fprintf(stderr, "TEST MOV: Failed to set DL value.\n");
		return 1;
	}

	int i = 0;
	int keys[] = { 12, 123, 1234 };
	char *vals[] = { "39", "1234", "4d2" };

	parse(glob, l_5);
	move(glob, NULL, BUF_SZ);

	parse(glob, l_6);
	move(glob, NULL, BUF_SZ);

	parse(glob, l_7);
	move(glob, NULL, BUF_SZ);

	mem_nodes_t *node = glob->mem->head;
	while (node) {
		if (node->addr != keys[i]) {
			fprintf(stderr,
				"TEST MOV: Order mismatch [%d] - [%d].\n",
				node->addr, keys[i]);
			return 1;
		}

		if (strcmp(node->val, vals[i])) {
			fprintf(stderr,
				"TEST MOV: Value mismatch [%s] - [%s].\n",
				node->val, vals[i]);
		}

		node = node->next;
		i++;
	}

	assert(i);
	fclose(fd);
	return 0;
}