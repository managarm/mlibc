#ifndef _QWORD_MOUNT_H
#define _QWORD_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

int mount(const char *source, const char *target,
    const char *filesystemtype, unsigned long mountflags,
    const void *data);
int umount(const char *target);

#ifdef __cplusplus
}
#endif

#endif // _QWORD_MOUNT_H
