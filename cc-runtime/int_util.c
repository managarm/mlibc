//===-- int_util.c - Implement internal utilities -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// NOTE: The definitions in this file are declared weak because we clients to be
// able to arbitrarily package individual functions into separate .a files. If
// we did not declare these weak, some link situations might end up seeing
// duplicate strong definitions of the same symbol.
//
// We can't use this solution for kernel use (which may not support weak), but
// currently expect that when built for kernel use all the functionality is
// packaged into a single library.

__attribute__((weak))
__attribute__((visibility("hidden")))
void __compilerrt_abort_impl(const char *file, int line, const char *function) {
  (void)file; (void)line; (void)function;
#if !__STDC_HOSTED__
  // Avoid depending on libc when compiling with -ffreestanding.
  __builtin_trap();
#else
  __builtin_abort();
#endif
}
