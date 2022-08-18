# ABI Breaks

This document lists the ABI breaks that were made in each mlibc major version.

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
