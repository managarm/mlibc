// This translation unit provides symbols for functions marked with __MLIBC_INLINE_DEFINITION.
// All headers with such functions must be included here.

#define __MLIBC_EMIT_INLINE_DEFINITIONS

#include <mlibc-config.h>

#include <bits/sysmacros.h>
#include <elf.h>

#ifndef MLIBC_BUILDING_RTLD
#include <math.h>
#endif
