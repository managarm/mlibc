//===-- powixf2.cpp - Implement __powixf2 ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __powixf2 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#ifndef CC_RUNTIME_NO_FLOAT

#if !_ARCH_PPC

#include "int_lib.h"

#if HAS_80_BIT_LONG_DOUBLE == 1

// Returns: a ^ b

COMPILER_RT_ABI xf_float __powixf2(xf_float a, int b) {
  const int recip = b < 0;
  xf_float r = 1;
  while (1) {
    if (b & 1)
      r *= a;
    b /= 2;
    if (b == 0)
      break;
    a *= a;
  }
  return recip ? 1 / r : r;
}

#endif

#endif

#endif
