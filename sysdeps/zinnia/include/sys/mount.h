#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

#define MNT_RDONLY   (1 << 0)
#define MNT_NOSUID   (1 << 1)
#define MNT_NOEXEC   (1 << 2)
#define MNT_RELATIME (1 << 3)
#define MNT_NOATIME  (1 << 4)
#define MNT_REMOUNT  (1 << 5)
#define MNT_FORCE    (1 << 6)

int mount(const char *type, const char *dir, int flags, void *data);
int unmount(const char *dir, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MOUNT_H */
