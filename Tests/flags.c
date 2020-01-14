/* Unit test for instructions that modify FLAG values. */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../Flags/flags.h"
#include "../Glob/glob.h"
#include "../Math/mathop.h"
#include "../Memory/mem.h"
#include "../Parser/parse.h"

int main(void)
{
	FILE *fd = fopen("Tests/ph", "r");
	if (!fd) {
		fprintf(stderr, "TEST: FLAGS - Could not open PH.\n");
		return 1;
	}

	glob_t *glob = init_glob(fd);
	if (!glob) {
		fprintf(stderr, "TEST: FLAGS - Glob is NULL.\n");
		return 1;
	}

	char l_1[] = "STC";
	char l_2[] = "STD";
	char l_3[] = "STI";
	char l_4[] = "CMC";
	char l_5[] = "CLC";
	char l_6[] = "CLD";
	char l_7[] = "CLI";
	char l_8[] = "MOV AX, 0";
	char l_9[] = "ADD AX, 1";
	char l_10[] = "MOV AX, -5";
	char l_11[] = "ADD AX, 5";

	parse(glob, l_1);
	set_flag(glob, NULL, BUF_SZ);
	if (glob->flags->cf != 1) {
		fprintf(stderr, "TEST: FLAGS - Could not set CF value.\n");
		return 1;
	}

	parse(glob, l_2);
	set_flag(glob, NULL, BUF_SZ);
	if (glob->flags->df != 1) {
		fprintf(stderr, "TEST: FLAGS - Could not set DF value.\n");
		return 1;
	}

	parse(glob, l_3);
	set_flag(glob, NULL, BUF_SZ);
	if (glob->flags->iif != 1) {
		fprintf(stderr, "TEST: FLAGS - Could not set IF value.\n");
		return 1;
	}

	parse(glob, l_4);
	cmc(glob, NULL, BUF_SZ);
	if (glob->flags->cf != 0) {
		fprintf(stderr, "TEST: FLAGS - Could not run CMC.\n");
		return 1;
	}

	parse(glob, l_5);
	clear_flag(glob, NULL, BUF_SZ);
	if (glob->flags->cf != 0) {
		fprintf(stderr, "TEST: FLAGS - Could not clear CF flag.\n");
		return 1;
	}

	parse(glob, l_6);
	clear_flag(glob, NULL, BUF_SZ);
	if (glob->flags->df != 0) {
		fprintf(stderr, "TEST: FLAGS - Could not clear DF flag.\n");
		return 1;
	}

	parse(glob, l_7);
	clear_flag(glob, NULL, BUF_SZ);
	if (glob->flags->iif != 0) {
		fprintf(stderr, "TEST: FLAGS - Could not clear IF flag.\n");
		return 1;
	}

	parse(glob, l_8);
	move(glob, NULL, BUF_SZ);
	if (!glob->flags->zf) {
		fprintf(stderr, "TEST: Flags - Move did not trigger zf.\n");
		return 1;
	}

	parse(glob, l_9);
	math_op(glob, NULL, BUF_SZ);
	if (strcmp(glob->registers->ax, "1")) {
		fprintf(stderr, "TEST: Flags - MOV AX failed.\n");
		return 1;
	}

	if (glob->flags->zf != 0) {
		fprintf(stderr, "TEST: Flags - zf did not change after MOV.\n");
		return 1;
	}

	parse(glob, l_10);
	move(glob, NULL, BUF_SZ);

	parse(glob, l_11);
	math_op(glob, NULL, BUF_SZ);
	if (strcmp(glob->registers->ax, "0")) {
		fprintf(stderr, "TEST: Flags - AX doesn't equal to "
				"0. Error parsing -ve numbers.\n");
		return 1;
	}

	if (glob->flags->zf != 1) {
		fprintf(stderr, "TEST: Flags - zf did not change after MOV.\n");
		return 1;
	}

	fclose(fd);
	return 0;
}