#ifndef _BITS_FEATURE_H
#define _BITS_FEATURE_H

// TODO: The files for this macro are chosen somewhat arbitrarily.
// TODO: Choose canonical files (like a bits/posix/alltypes.h)
#define __MLIBC_ANSI_OPTION __has_include(<bits/ansi/seek.h>)
#define __MLIBC_POSIX_OPTION __has_include(<bits/posix/file.h>)

#endif // _BITS_FEATURE_H
