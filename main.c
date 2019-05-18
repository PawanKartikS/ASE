/**
 * @file: main.c
 * @desc: Front-end for ASE.
 * 
 * @author: Pawan Kartik
 *          Manipal University - 2019
 */

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bind.h"
#include "display.h"
#include "glob.h"
#include "mem.h"
#include "parse.h"
#include "stack.h"
#include "tengine.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void parse_args(glob_t *glob, int argc, char **argv, args_t *p_args) {
	int opt;
	int idx = 0;
	struct option long_opt[] = 
	{
		{"all-flags", no_argument, 0, 'a'},
		{"no-warns",  no_argument, 0, 'w'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "ab:dfhmrsvw", long_opt, &idx)) != -1) {
		switch (opt) {
		case 'a': p_args->f  = p_args->m = p_args->r = p_args->s = 1; break;

		/**
		 * TODO
		 * Allow user to set a break point.
		 */
		case 'b': glob->bpnt = (int)strtol(optarg, NULL, 0); break;
		
		/* Debug Mode */
		case 'd': glob->debug_mode = 1; break;

		case 'f': p_args->f  = 1; break;
		case 'h': p_args->h  = 1; break;
		case 'm': p_args->m  = 1; break;
		case 'r': p_args->r  = 1; break;
		case 's': p_args->s  = 1; break;
		case 'v': p_args->v  = 1; break;
		
		/* Turn off warnings */
		case 'w': glob->mem->warned = 1; break;
		}
	}

	for (; optind < argc; optind++) {
		if (!strstr(argv[optind], ".asm")) {
			fprintf(stderr, "Ignoring extra argument: %s\n", argv[optind]);
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Error: Please specify minimum number [1] of args.\n");
		fprintf(stderr, "ASE [Source File] [Arguments]\n\n");
		show_flags();
		return 1;
	}

	if (argc == 2) {
		if (!strstr(argv[1], "-")) {
			fprintf(stderr, "Warning: Missing program flag(s)?\n");
		}
	}

	args_t args_ = {0};
	table_t *table = init_table();
	bind_calls(table);
	FILE *fd = fopen(argv[1], "r");
	
	if (!fd) {
		fprintf(stderr, "Could not open specified file.\n");
		return 1;
	}

	int flag = 0, exec = 1;
	char line[1024];
	glob_t *glob = init_glob(fd);
	memset(line, 0, sizeof(line));
	parse_args(glob, argc, argv, &args_);

	if(glob->debug_mode) {
		printf("Debug Mode. Press 'c' to continue.\n\n");
	}
	
	while (fgets(line, sizeof(line), fd) != NULL) {
		if (should_skip_ln(line)) {
			continue;
		}
		
		if (!parse_line(glob, line)) {
			fprintf(stderr, "Could not parse line.\n");
			flag = 1;
			exec = 0;
		}

		int ret = call_by_name(table, glob, NULL, (unsigned long)BUF_SZ);
		if (ret == -1) {
			exec = 0;
		}

		if (!ret) {
			flag = 1;
			exec = 0;
		}

		if(exec){
			if (glob->debug_mode) {
				printf("Evaluating: %s %s %s\n", glob->tokens[0], glob->tokens[1],
					glob->tokens[2]);

				char ch = getchar();
				while(ch != 'c'){
					ch = getchar();
				}

				display(glob, args_);
			}
		} else {
			break;
		}
	}

	if (flag) {
		fprintf(stderr, "Emulator halted due to an error in line %d. State preserved.\n\n",
			++glob->c_line);
	}

	if(glob->debug_mode){
		printf("\n\nResult\n" ANSI_COLOR_GREEN);
	}
	/* Program ends here. */
	display(glob, args_);

	/* Close existing file handles and clear out alloc'ed memory. */
	fclose(fd);
	destroy_glob(glob);
	destroy_table(table);

	return flag;
}
