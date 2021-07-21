#ifndef MLIBC_ALL_SYSDEPS
#define MLIBC_ALL_SYSDEPS

#include <mlibc-config.h>
#include <internal-config.h>

#ifdef __MLIBC_ANSI_OPTION
#	include <mlibc/ansi-sysdeps.hpp>
#endif // __MLIBC_ANSI_OPTION

#ifdef __MLIBC_POSIX_OPTION
#	include <mlibc/posix-sysdeps.hpp>
#endif // __MLIBC_POSIX_OPTION

#ifdef __MLIBC_LINUX_OPTION
#	include <mlibc/linux-sysdeps.hpp>
#endif // __MLIBC_LINUX_OPTION

#ifdef __MLIBC_GLIBC_OPTION
#	include <mlibc/glibc-sysdeps.hpp>
#endif // __MLIBC_GLIBC_OPTION

#ifdef MLIBC_BUILDING_RTDL
#	include <mlibc/rtdl-sysdeps.hpp>
#endif // MLIBC_BUILDING_RTDL

#include <mlibc/internal-sysdeps.hpp>

#endif // MLIBC_ALL_SYSDEPS
