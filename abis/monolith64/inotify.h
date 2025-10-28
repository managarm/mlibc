#ifndef _ABIBITS_INOTIFY_H
#define _ABIBITS_INOTIFY_H

#include <mlibc-config.h>

#if !__MLIBC_LINUX_OPTION
#  error "<sys/inotify.h> is inherently Linux specific. Enable the Linux option or do not use this header."
#endif /* !__MLIBC_LINUX_OPTION */

#include <abi-bits/fcntl.h>

#define IN_CLOEXEC O_CLOEXEC
#define IN_NONBLOCK O_NONBLOCK

#endif /* _ABIBITS_INOTIFY_H */
