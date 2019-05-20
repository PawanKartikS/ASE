/**
 * @file: display.c
 * @desc: Defines functions that put data from memory, registers and
 *        stack onto the display.
 */ 

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"

#define BUILD 1905

void show_flags() {
  fprintf(stderr, "Supported flags: \n\
		-a : Enable all (below) emulator specified flags \n\
		-f : Show flag contents \n\
		-h : Show help (this) screen \n\
		-m : Show memory contents \n\
		-r : Show register contents \n\
		-s : Show stack contents\n\
		-v : Show version info\n");
}

/**
 * @desc  : Display data from memory, registers and stack.
 * @param : glob -
 * @return: void
 */ 
void display(glob_t *glob, args_t p_args) {
	if (!glob) {
		return;
	}

	if (p_args.f && glob->flags) {
		printf("Flags:\n");
		printf("[CF]:[%d]\n",   glob->flags->cf);
		printf("[DF]:[%d]\n",   glob->flags->df);
		printf("[IF]:[%d]\n",   glob->flags->iif);
		printf("[OF]:[%d]\n",   glob->flags->of);
		printf("[PF]:[%d]\n",   glob->flags->pf);
		printf("[SF]:[%d]\n",   glob->flags->sf);
		printf("[ZF]:[%d]\n\n", glob->flags->zf);
	}

	if (p_args.r && glob->registers) {
		printf("Register:\n");
		printf("[AX]:[%s]\n",   glob->registers->ax);
		printf("[BX]:[%s]\n",   glob->registers->bx);
		printf("[CX]:[%s]\n",   glob->registers->cx);
		printf("[DX]:[%s]\n\n", glob->registers->dx);
	}

	if (p_args.s && glob->stack) {
		if (glob->stack->top == -1) {
			goto skip;	
		}

		char **ptr = &glob->stack->arr[0];
		if (*ptr) {
			printf("Stack:\n");
		}

		while (*ptr) {
			char **ref = ptr;
			printf("[%p]:[%s]\n", (void*)&*ref, *ptr++);
			if (!*ptr)
				printf("\n");
		}
	}

	skip:
	if (p_args.m && glob->mem) {
		printf("Memory:\n");
		mem_nodes_t *node = glob->mem->head;
		
		while (node && glob) {
			printf("[%d:%d] - [%s]\n", node->seg, node->offset, node->val);
			node = node->next;
		}
	}

	if (p_args.h) {
		show_flags();
	}

	if (p_args.v) {
		printf("Build : %d\nAuthor: Pawan Kartik\n", BUILD);
	}
}