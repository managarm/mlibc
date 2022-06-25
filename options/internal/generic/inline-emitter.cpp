// This translation unit provides symbols for functions marked with __MLIBC_INLINE_DEFINITION.
// All headers with such functions must be included here.

#define __MLIBC_EMIT_INLINE_DEFINITIONS

#include <mlibc-config.h>

#include <elf.h>

#ifdef __MLIBC_LINUX_OPTION
#include <sys/sysmacros.h>
#endif

#ifndef MLIBC_BUILDING_RTDL
#include <math.h>
#endif
