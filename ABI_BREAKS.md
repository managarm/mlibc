# ABI Breaks

This document lists the ABI breaks that were made in each mlibc major version.

## Version 5, 6

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
