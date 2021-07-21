
#ifndef _LINUX_MAGIC_H
#define _LINUX_MAGIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Networking file systems */
#define AFS_SUPER_MAGIC 0x5346414F
#define CODA_SUPER_MAGIC 0x73757245
#define NCP_SUPER_MAGIC 0x564c
#define NFS_SUPER_MAGIC 0x6969
#define SMB_SUPER_MAGIC 0x517B

/* Linux in-memory filesystems */
#define PROC_SUPER_MAGIC 0x9fa0
#define RAMFS_MAGIC 0x858458f6
#define TMPFS_MAGIC 0x01021994
#define USBDEVICE_SUPER_MAGIC 0x9fa2

#ifdef __cplusplus
}
#endif

#endif // _LINUX_MAGIC_H

