#include "display.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void display(glob_t *glob, args_t p_args)
{
	if (!glob) {
		return;
	}

	if (p_args.f && glob->flags) {
		printf("Flags:\n");
		printf("[CF]:[%d]\n", glob->flags->cf);
		printf("[DF]:[%d]\n", glob->flags->df);
		printf("[IF]:[%d]\n", glob->flags->iif);
		printf("[OF]:[%d]\n", glob->flags->of);
		printf("[PF]:[%d]\n", glob->flags->pf);
		printf("[SF]:[%d]\n", glob->flags->sf);
		printf("[ZF]:[%d]\n\n", glob->flags->zf);
	}

	if (p_args.h) {
		show_flags();
	}

	if (p_args.l) {
		if (glob->idx) {
			printf("User specified labels:\n");
		}

		for (int i = 0; i < glob->idx; i++) {
			printf("[%s]:[%d]\n", glob->label_locs[i].label,
			       glob->label_locs[i].line);
		}
	}

	if (p_args.m && glob->mem) {
		mem_nodes_t *node = glob->mem->head;

		if (node && glob) {
			printf("Memory:\n");

			while (node && glob) {
				printf("[%d:%d] - [%s]\n", node->seg,
				       node->offset, node->val);
				node = node->next;
			}
		}
	}

	if (p_args.r && glob->registers) {
		printf("Register:\n");
		printf("[AX]:[%s]\n", glob->registers->ax);
		printf("[BX]:[%s]\n", glob->registers->bx);
		printf("[CX]:[%s]\n", glob->registers->cx);
		printf("[DX]:[%s]\n\n", glob->registers->dx);
	}

	if (p_args.s && glob->stack) {
		int top = glob->stack->top;
		while (top >= 0) {
			printf("[%p]:[%s]\n", (void *)&glob->stack->arr[top],
			       glob->stack->arr[top]);
			top--;
		}
	}

	if (p_args.v) {
		printf("Build : %d\nAuthor: Pawan Kartik\n", BUILD);
	}
}

void parse_args(glob_t *glob, int argc, char **argv, args_t *args)
{
	for (int i = 2; i < argc; i++) {
		/* clang-format off */
        if (!strcmp(argv[i], "-a")) {
            args->f = args->m = args->r = args->s = 1;
            continue;
        }
        
        if (!strcmp(argv[i], "-d")) {
            glob->debug = 1;
            continue;
        }

        if (!strcmp(argv[i], "-f")) {
            args->f = 1;
            continue;
        }

        if (!strcmp(argv[i], "-h")) {
            args->h = 1;
            continue;
        }

        if (!strcmp(argv[i], "-l")) {
            args->l = 1;
            continue;
        }

        if (!strcmp(argv[i], "-m")) {
            args->m = 1;
            continue;
        }

        if (!strcmp(argv[i], "-r")) {
            args->r = 1;
            continue;
        }

        if (!strcmp(argv[i], "-s")) {
            args->s = 1;
            continue;
        }

        if (!strcmp(argv[i], "-v")) {
            args->v = 1;
            continue;
        }

        printf("Skipping invalid arg: %s.\n\n", argv[i]);
		/* clang-format on */
	}
}

void show_flags()
{
	fprintf(stderr, "Supported flags: \n\
        -a : Enable all (below) emulator specified flags \n\
        -d : Enable debug mode \n\
        -f : Show flag contents \n\
        -h : Show help (this) screen \n\
        -l : Display declared labels with their line \n\
        -m : Show memory contents \n\
        -r : Show register contents \n\
        -s : Show stack contents \n\
        -v : Show version info \n");
}