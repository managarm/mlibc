#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

#define MNT_EXT 1
#define MNT_FAT 2
#define MNT_DEV 3

#define MS_RDONLY   (1 << 0)
#define MS_REMOUNT  (1 << 1)
#define MS_RELATIME (1 << 2)
#define MS_NOATIME  (1 << 3)

#define MNT_FORCE 1

int mount(const char *source, const char *target, int type, int flags);
int umount(const char *target, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MOUNT_H */
