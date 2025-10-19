# ABI Breaks

This document lists the ABI breaks that were made in each mlibc major version.

## Version 6

Numerous ABI breaks. These were not properly logged, and are therefore missing here. Pending update, if one ever comes.

- [#1458](https://github.com/managarm/mlibc/pull/1458): mlibc only defined `fenv_t` for x86(_64) before, new we define it correctly for all supported architectures.
- [#1458](https://github.com/managarm/mlibc/pull/1458): due to incorrect macro usage, `LONG_BIT` was previously always defined to 64, even on 32-bit architectures. Now, it is set to 32 on 32-bit architectures.
- [#1458](https://github.com/managarm/mlibc/pull/1458): `PRI*FAST*` macros had values that caused `-Wformat` warnings on 32-bit architectures, which got fixed.
- [#1458](https://github.com/managarm/mlibc/pull/1458): `ND_NA_FLAG_*` macros were incorrectly not byte-swapped on big-endian architectures.
- [#1458](https://github.com/managarm/mlibc/pull/1458): some members of `struct link_map` were always using `Elf64_Addr` instead of the correct `ElfW(Addr)` type.
- [#1459](https://github.com/managarm/mlibc/pull/1459): the sizing of `blksize_t` and `nlink_t` was incorrect, which lead to incorrect layouts of Linux's `struct stat` for most architectures (i.e. non-x86)
- [#1459](https://github.com/managarm/mlibc/pull/1459): the layout of `struct statvfs` and `struct statvfs64` was incorrect
- [#1459](https://github.com/managarm/mlibc/pull/1459): some `O_*` and `F_*` macros in `<fcntl.h>` were incorrect on some architectures and were fixed.
- [#1459](https://github.com/managarm/mlibc/pull/1459): values of `POLLWRNORM` and `POLLWRBAND` were incorrect on m68k
- [#1459](https://github.com/managarm/mlibc/pull/1459): fixed incorrect layout of `struct shmid_ds`
- [#1459](https://github.com/managarm/mlibc/pull/1459): the layouts of `sigcontext`, `ucontext_t` and `mcontext_t` were incorrect in multiple architectures and got fixed.
- [#1459](https://github.com/managarm/mlibc/pull/1459): the layout of `struct msqid64_ds` was incorrect due to a silently incorrect ifdef, which was fixed but broke ABI.
- [#1460](https://github.com/managarm/mlibc/pull/1460): the size of the `d_name` member of `dirent` was changed to `NAME_MAX+1` in order to align with the maximum permitted value size by POSIX.
- [#1460](https://github.com/managarm/mlibc/pull/1460): the `wcsxfrm` and `confstr` function had incorrect return types, which were fixed.
- [#1460](https://github.com/managarm/mlibc/pull/1460): `PRIO_*` values were fixed to align with the values Linux syscalls expect.
- [#1460](https://github.com/managarm/mlibc/pull/1460): the alignment of `fd_set` was fixed.
- [#1460](https://github.com/managarm/mlibc/pull/1460): the layout of `struct semid_ds` was fixed.
- [#1460](https://github.com/managarm/mlibc/pull/1460): `<sys/user.h>` previously always defined structs for x86_64, which were changed to now be correct for all supported architectures.
- [#1460](https://github.com/managarm/mlibc/pull/1460): `off_t` and `off64_t` are now always 64-bit instead of `long`.
- [#1460](https://github.com/managarm/mlibc/pull/1460): fixed the layout of `struct rtentry` on 32-bit architectures.
- [#1460](https://github.com/managarm/mlibc/pull/1460): `useconds_t` was turned into a 32-bit value, as it only needs to hold microsecond values worth up to a second.
- [#1492](https://github.com/managarm/mlibc/pull/1492): changes the values of `LC_*` macros and `nl_item` values to match glibc, so that glibc locale files can be consumed.
- [#1492](https://github.com/managarm/mlibc/pull/1492): fix `struct epoll_event` alignment on x86

## Version 5

Numerous ABI breaks. These were not properly logged, and are therefore missing here. Pending update, if one ever comes.

## Version 4

- [#814](https://github.com/managarm/mlibc/pull/814): `struct timex`'s `long int tai` changed to the correct `int tai`, and `int __padding[11]` got appended to the struct.
- [#816](https://github.com/managarm/mlibc/pull/816): `sys_wait4` on Linux now correctly returns a `pid_t`, not an `int` as previously.
- [#816](https://github.com/managarm/mlibc/pull/816): All `MS_*` macros of the `sys/mount.h` header were adjusted to match linux.
- [#816](https://github.com/managarm/mlibc/pull/816): `struct epoll_event` now gets correctly packed on `x86_64`.
- [#819](https://github.com/managarm/mlibc/pull/819): `str(n)dupa` is now defined as a pure macro, and not as a macro that points to a function.
- [#828](https://github.com/managarm/mlibc/pull/828): Linux-specific functions previously included in the posix option in `pthreads.h` and `sched.h` are not correctly guarded behind the linux option.
- [#828](https://github.com/managarm/mlibc/pull/828): The `CPU_*` macros of `sched.h` have been rewritten to resolve to internal mlibc implementations, and are now correctly guarded behind the linux option.
- [#735](https://github.com/managarm/mlibc/pull/735): `sched_getcpu` and `setns` were previously mistakenly C++-mangled and not declared, which has now been rectified.

## Version 3

- [#728](https://github.com/managarm/mlibc/pull/728):
  The macros `CMSG_{LEN,SPACE,DATA}` were not accounting for padding between
  `struct cmsghdr` and it's respective data. This manifested itself as some
  parts of control data being skipped on platforms where `struct cmsghdr` is
  not divisible by `alignof(size_t)`.
- [#452](https://github.com/managarm/mlibc/pull/452): The functions `FD_{CLR,ISSET,SET,ZERO}` were renamed to `__FD_{CLR,ISSET,SET,ZERO}` and replaced by macros to match Wine's assumptions.
- [#511](https://github.com/managarm/mlibc/pull/511): Musl's regex engine was added, implementing `regcomp` and `regexec`. This required some changes to the `regex_t` struct.
- [#504](https://github.com/managarm/mlibc/pull/504): In the Linux ABI, a `domainname` member was added to `struct utsname`, which is a glibc extension.
- [#311](https://github.com/managarm/mlibc/pull/311): Added all necessary fields in `pthread_attr_t` required for implementing all `pthread_attr` functions.
- [#652](https://github.com/managarm/mlibc/pull/652): The ABI of `struct statfs` and `struct statvfs` was changed to match Linux. `socklen_t` was also changed from `unsigned long` to `unsigned int`.
- [#658](https://github.com/managarm/mlibc/pull/648): In the Linux ABI, `cc_t` was changed from an `unsigned int` to an `unsigned char`.
- [#679](https://github.com/managarm/mlibc/pull/679): The `struct glob_t` received some additional members to bring it up to par with glibc.
