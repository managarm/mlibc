#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

#define MNT_FORCE 1

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data);
int umount(const char *target);
int umount2(const char *target, int flags);

#ifdef __cplusplus
}
#endif

#endif // _SYS_MOUNT_H
