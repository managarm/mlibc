#ifndef MLIBC_MAP_FLAGS_H
#define MLIBC_MAP_FLAGS_H

#define PROT_NONE 0x00
#define PROT_READ 0x01
#define PROT_WRITE 0x02
#define PROT_EXEC 0x04

#define MAP_PRIVATE 0x01
#define MAP_SHARED 0x02
#define MAP_FIXED 0x04

// Linux extensions:
#define MAP_ANON 0x08
#define MAP_ANONYMOUS 0x08

#endif // MLIBC_MAP_FLAGS_H
