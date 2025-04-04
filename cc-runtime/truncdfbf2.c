//===-- lib/truncdfbf2.c - double -> bfloat conversion ------------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef CC_RUNTIME_NO_FLOAT

#if defined(__x86_64__) || defined(__aarch64__) || (defined(__riscv) && defined(__clang__))

#define SRC_DOUBLE
#define DST_BFLOAT
#include "fp_trunc_impl.inc"

COMPILER_RT_ABI dst_t __truncdfbf2(double a) { return __truncXfYf2__(a); }

#endif

#endif
