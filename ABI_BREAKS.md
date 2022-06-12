# ABI Breaks

This document lists the ABI breaks that were made in each mlibc major version.

## Version 3

- [#452](https://github.com/managarm/mlibc/pull/452): The functions `FD_{CLR,ISSET,SET,ZERO}` were renamed to `__FD_{CLR,ISSET,SET,ZERO}` and replaced by macros to match Wine's assumptions.
- [#511](https://github.com/managarm/mlibc/pull/511): Musl's regex engine was added, implementing `regcomp` and `regexec`. This required some changes to the `regex_t` struct.
