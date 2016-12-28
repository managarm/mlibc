#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data);

#ifdef __cplusplus
}
#endif

#endif // _SYS_MOUNT_H
