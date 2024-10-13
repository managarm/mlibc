#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

#define MNT_EXT 1
#define MNT_FAT 2

#define MS_RDONLY   0b001
#define MS_REMOUNT  0b010
#define MS_RELATIME 0b100

#define MNT_FORCE 1

int mount(const char *source, const char *target, int type, int flags);
int umount(const char *target, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MOUNT_H */
