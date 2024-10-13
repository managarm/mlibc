
#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/off_t.h>

#include <bits/posix/id_t.h>
#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/pid_t.h>

#include <abi-bits/mode_t.h>
#include <abi-bits/dev_t.h>
#include <abi-bits/ino_t.h>
#include <abi-bits/blksize_t.h>
#include <abi-bits/blkcnt_t.h>
#include <abi-bits/nlink_t.h>

#include <bits/ansi/time_t.h>
#include <abi-bits/suseconds_t.h>

#include <abi-bits/fsblkcnt_t.h>
#include <abi-bits/fsfilcnt_t.h>
#include <bits/posix/fd_set.h>

#include <stdint.h>

#include <sys/select.h>

typedef unsigned int u_int;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long int u_long;
typedef char *caddr_t;
typedef off64_t loff_t;

typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;

/* BSD extensions */
typedef int64_t quad_t;
typedef uint64_t u_quad_t;

#endif /* _SYS_TYPES_H */

