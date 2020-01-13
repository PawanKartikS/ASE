/**
 * @file: main.c
 * @desc: Front-end for ASE.
 *
 * @author: Pawan Kartik
 *          Manipal University - 2019
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bind/bind.h"
#include "Display/display.h"
#include "Glob/glob.h"
#include "Memory/mem.h"
#include "Parser/parse.h"
#include "Stack/stack.h"
#include "TEngine/tengine.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: Please specify minimum "
                        "number [1] of args.\n");
        fprintf(stderr, "ASE [Source File] [Arguments]\n\n");
        show_flags();
        return 1;
    }

    if (argc == 2) {
        if (!strstr(argv[1], "-")) {
            fprintf(stderr, "Warning: Missing program flag(s)?\n");
        }
    }

    if (!strcmp(argv[1], "-v")) {
        printf("Build : %d\nAuthor: Pawan Kartik\n", BUILD);
        return 0;
    }

    args_t args = {0};
    table_t *table = init_table();
    bind_calls(table);
    FILE *fd = fopen(argv[1], "r");

    if (!fd) {
        fprintf(stderr, "Could not open specified file.\n");
        return 1;
    }

    char line[1024] = {0};
    int flag = 0, exec = 1;
    glob_t *glob = init_glob(fd);
    parse_args(glob, argc, argv, &args);

    if (glob->debug) {
        printf("Debug Mode. Press 'c' to continue.\n\n");
    }

    while (fgets(line, sizeof(line), fd) != NULL) {
        if (should_skip_ln(line)) {
            glob->c_line++;
            continue;
        }

        if (!parse(glob, line)) {
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

        if (exec) {
            /**
             * Debug mode is called only if there are no bad
             * returns.
             */
            if (glob->debug) {
                printf("Evaluating: %s %s %s\n", glob->tokens[0],
                       glob->tokens[1], glob->tokens[2]);

                char ch = getchar();
                while (ch != 'c') {
                    ch = getchar();
                }

                display(glob, args);
            }

        } else {
            /**
             * When exec = 0, we have a bad return from
             * above function calls, hence we exit the while
             * loop.
             */
            break;
        }
    }

    if (flag) {
        fprintf(stderr,
                "Emulator halted due to an error in line "
                "%d. State preserved.\n\n",
                ++glob->c_line);
    }

    if (glob->debug) {
        printf("\n\nResult\n" TERM_GREEN);
    }

    /* Program ends here. */
    display(glob, args);

    /* clearing alloc'ed memory. */
    destroy_glob(glob);
    destroy_table(table);

    return flag;
}