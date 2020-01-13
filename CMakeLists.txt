cmake_minimum_required(VERSION 3.10)
project(ASE C)

add_executable(Main
                    Bind/bind.c
                    Display/display.c
                    Flags/flags.c
                    Glob/glob.c
                    main.c
                    Math/mathop.c
                    Memory/mem.c
                    Parser/parse.c
                    Stack/stack.c
                    TEngine/tengine.c)