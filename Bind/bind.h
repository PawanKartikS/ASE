/**
 * @file: bind.h
 * @desc: Declares function that helps in binding
 * instruction set to strings.
 */

#pragma once

#include "../Flags/flags.h"
#include "../Math/mathop.h"
#include "../Memory/mem.h"
#include "../Stack/stack.h"
#include "../TEngine/tengine.h"

void bind_calls(table_t *table);