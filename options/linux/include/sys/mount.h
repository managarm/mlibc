#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MS_RDONLY 1
#define MS_NOSUID 2
#define MS_NODEV 4
#define MS_NOEXEC 8
#define MS_SYNCHRONOUS 16
#define MS_REMOUNT 32
#define MS_MANDLOCK 64
#define MS_DIRSYNC 128
#define MS_NOSYMFOLLOW 256
#define MS_NOATIME 1024
#define MS_NODIRATIME 2048
#define MS_BIND 4096
#define MS_MOVE 8192
#define MS_REC 16384
#define MS_SILENT 32768
#define MS_POSIXACL (1 << 16)
#define MS_UNBINDABLE (1 << 17)
#define MS_PRIVATE (1 << 18)
#define MS_SLAVE (1 << 19)
#define MS_SHARED (1 << 20)
#define MS_RELATIME (1 << 21)
#define MS_KERNMOUNT (1 << 22)
#define MS_I_VERSION (1 << 23)
#define MS_STRICTATIME (1 << 24)
#define MS_LAZYTIME (1 << 25)
#define MS_NOREMOTELOCK (1 << 27)
#define MS_NOSEC (1 << 28)
#define MS_BORN (1 << 29)
#define MS_ACTIVE (1 << 30)
#define MS_NOUSER (1 << 31)

#define MNT_FORCE 1
#define MNT_DETACH 2
#define MNT_EXPIRE 4
#define UMOUNT_NOFOLLOW 8

#undef BLKROSET
#define BLKROSET _IO(0x12, 93)
#undef BLKROGET
#define BLKROGET _IO(0x12, 94)
#undef BLKRRPART
#define BLKRRPART _IO(0x12, 95)
#undef BLKGETSIZE
#define BLKGETSIZE _IO(0x12, 96)
#undef BLKFLSBUF
#define BLKFLSBUF _IO(0x12, 97)
#undef BLKRASET
#define BLKRASET _IO(0x12, 98)
#undef BLKRAGET
#define BLKRAGET _IO(0x12, 99)
#undef BLKFRASET
#define BLKFRASET _IO(0x12, 100)
#undef BLKFRAGET
#define BLKFRAGET _IO(0x12, 101)
#undef BLKSECTSET
#define BLKSECTSET _IO(0x12, 102)
#undef BLKSECTGET
#define BLKSECTGET _IO(0x12, 103)
#undef BLKSSZGET
#define BLKSSZGET _IO(0x12, 104)
#undef BLKBSZGET
#define BLKBSZGET _IOR(0x12, 112, size_t)
#undef BLKBSZSET
#define BLKBSZSET _IOW(0x12, 113, size_t)
#undef BLKGETSIZE64
#define BLKGETSIZE64 _IOR(0x12, 114, size_t)

#ifndef __MLIBC_ABI_ONLY

int mount(const char *__source, const char *__target,
		const char *__fstype, unsigned long __flags, const void *__data);
int umount(const char *__target);
int umount2(const char *__target, int __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MOUNT_H */
