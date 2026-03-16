
// SPDX-License-Identifier: MIT

#pragma once

#include <abi-bits/signal.h>
#include <abi-bits/stat.h>
#include <bits/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// C enum.
typedef enum {
#define _SYSCALL_DEF(__num, __enum, __name, __returns, ...) __enum = __num,
#include <abi-bits/syscall_defs.inc>
} __syscall_t;

// C declarations.
#define _SYSCALL_DEF(__num, __enum, __name, __returns, ...) __returns __name(__VA_ARGS__);
#include <abi-bits/syscall_defs.inc>

#ifdef __cplusplus
} // extern "C"
#endif
