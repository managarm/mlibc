#ifndef MLIBC_ALL_SYSDEPS
#define MLIBC_ALL_SYSDEPS

#include <mlibc-config.h>
#include <internal-config.h>

/* The ANSI option is always enabled. */
#include <mlibc/ansi-sysdeps.hpp>

#if __MLIBC_POSIX_OPTION
#	include <mlibc/posix-sysdeps.hpp>
#endif /* __MLIBC_POSIX_OPTION */

#if __MLIBC_LINUX_OPTION
#	include <mlibc/linux-sysdeps.hpp>
#endif /* __MLIBC_LINUX_OPTION */

#if __MLIBC_GLIBC_OPTION
#	include <mlibc/glibc-sysdeps.hpp>
#endif /* __MLIBC_GLIBC_OPTION */

#if __MLIBC_BSD_OPTION
#	include <mlibc/bsd-sysdeps.hpp>
#endif /* __MLIBC_BSD_OPTION */

#if __MLIBC_LINUX_EPOLL_OPTION
#	include <mlibc/linux-epoll-sysdeps.hpp>
#endif /* __MLIBC_LINUX_EPOLL_OPTION */

#if __MLIBC_LINUX_TIMERFD_OPTION
#	include <mlibc/linux-timerfd-sysdeps.hpp>
#endif /* __MLIBC_LINUX_TIMERFD_OPTION */

#if __MLIBC_LINUX_SIGNALFD_OPTION
#	include <mlibc/linux-signalfd-sysdeps.hpp>
#endif /* __MLIBC_LINUX_SIGNALFD_OPTION */

#if __MLIBC_LINUX_EVENTFD_OPTION
#	include <mlibc/linux-eventfd-sysdeps.hpp>
#endif /* __MLIBC_LINUX_EVENTFD_OPTION */

#if __MLIBC_LINUX_REBOOT_OPTION
#	include <mlibc/linux-reboot-sysdeps.hpp>
#endif /* __MLIBC_LINUX_REBOOT_OPTION */

#if MLIBC_BUILDING_RTLD
#	include <mlibc/rtld-sysdeps.hpp>
#endif /* MLIBC_BUILDING_RTLD */

#include <mlibc/internal-sysdeps.hpp>

#endif /* MLIBC_ALL_SYSDEPS */
