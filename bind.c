#include "bind.h"
#include <assert.h>

void bind_calls(table_t *table) {
    assert(register_entry(table, "ADD", math_op, 2));
    assert(register_entry(table, "CLC", clear_flag, 0));
    assert(register_entry(table, "CLD", clear_flag, 0));
    assert(register_entry(table, "CLI", clear_flag, 0));
    assert(register_entry(table, "CMC", cmc, 0));
    assert(register_entry(table, "CMP", math_op, 2));
    assert(register_entry(table, "DEC", unary, 1));
    assert(register_entry(table, "HLT", hlt, 0));
    assert(register_entry(table, "IN", port, 2));
    assert(register_entry(table, "INC", unary, 1));
    assert(register_entry(table, "JCXZ", jump_cx, 1));
    assert(register_entry(table, "JC", jump_jx, 1));
    assert(register_entry(table, "JE", jump_jx, 1));
    assert(register_entry(table, "JNC", jump_jnx, 1));
    assert(register_entry(table, "JNE", jump_jnx, 1));
    assert(register_entry(table, "JP", jump_jx, 1));
    assert(register_entry(table, "JPE", jump_jx, 1));
    assert(register_entry(table, "JMP", jump, 1));
    assert(register_entry(table, "LAHF", lahf, 0));
    assert(register_entry(table, "MOV", move, 2));
    /* assert(register_entry(table, "MUL", math_op, 1)); */
    assert(register_entry(table, "NEG", neg, 1));
    assert(register_entry(table, "NOP", nop, 0));
    assert(register_entry(table, "ORG", org, 1));
    assert(register_entry(table, "OUT", port, 2));
    assert(register_entry(table, "POP", pop, 1));
    assert(register_entry(table, "PUSH", push, 1));
    assert(register_entry(table, "SAHF", sahf, 0));
    assert(register_entry(table, "STC", set_flag, 0));
    assert(register_entry(table, "STD", set_flag, 0));
    assert(register_entry(table, "STI", set_flag, 0));
    assert(register_entry(table, "SUB", math_op, 2));
    assert(register_entry(table, "XCHG", xchg, 2));
    /* clang-format off */
}
