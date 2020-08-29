#ifndef MLIBC_ALL_SYSDEPS
#define MLIBC_ALL_SYSDEPS

#include <bits/feature.h>
#include <internal-config.h>

#if __MLIBC_ANSI_OPTION
#	include <mlibc/ansi-sysdeps.hpp>
#endif // __MLIBC_ANSI_OPTION

#if __MLIBC_POSIX_OPTION
#	include <mlibc/posix-sysdeps.hpp>
#endif // __MLIBC_POSIX_OPTION

#if __MLIBC_LINUX_OPTION
#	include <mlibc/linux-sysdeps.hpp>
#endif // __MLIBC_LINUX_OPTION

#ifdef MLIBC_BUILDING_RTDL
#	include <mlibc/rtdl-sysdeps.hpp>
#endif // MLIBC_BUILDING_RTDL

// TODO(geert): Make glibc optional
// Fixes this hack which works around rtdl not
// including the glibc option
#ifndef MLIBC_BUILDING_RTDL
#	include <mlibc/glibc-sysdeps.hpp>
#endif // !MLIBC_BUILDING_RTDL
#include <mlibc/internal-sysdeps.hpp>

#endif // MLIBC_ALL_SYSDEPS
