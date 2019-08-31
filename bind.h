/**
 * @file: bind.h
 * @desc: Declares function that helps in binding
 * instruction set to strings.
 */

#pragma once

#include "flags.h"
#include "mathop.h"
#include "mem.h"
#include "stack.h"
#include "tengine.h"

void bind_calls(table_t *table);