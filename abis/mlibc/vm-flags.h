#ifndef _ABIBITS_MMAP_FLAGS_H
#define _ABIBITS_MMAP_FLAGS_H

#define PROT_NONE  0x00
#define PROT_READ  0x01
#define PROT_WRITE 0x02
#define PROT_EXEC  0x04

#define MAP_FAILED ((void *)(-1))
#define MAP_FILE      0x00
#define MAP_PRIVATE   0x01
#define MAP_SHARED    0x02
#define MAP_FIXED     0x04
#define MAP_ANON      0x08
#define MAP_ANONYMOUS 0x08
#define MAP_NORESERVE 0x10
#define MAP_FIXED_NOREPLACE 0x20

#define MS_ASYNC 0x01
#define MS_SYNC 0x02
#define MS_INVALIDATE 0x04

#define MCL_CURRENT 0x01
#define MCL_FUTURE 0x02

#define POSIX_MADV_NORMAL 1
#define POSIX_MADV_SEQUENTIAL 2
#define POSIX_MADV_RANDOM 3
#define POSIX_MADV_DONTNEED 4
#define POSIX_MADV_WILLNEED 5

#define MADV_NORMAL 0
#define MADV_RANDOM 1
#define MADV_SEQUENTIAL 2
#define MADV_WILLNEED 3
#define MADV_DONTNEED 4
#define MADV_FREE 8

/* Linux extensions: */
#define MREMAP_MAYMOVE 1
#define MREMAP_FIXED 2

#define MFD_CLOEXEC 1U
#define MFD_ALLOW_SEALING 2U

#endif /* _ABIBITS_MMAP_FLAGS_H */
