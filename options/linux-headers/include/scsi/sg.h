
#ifndef _LINUX_SCSI_SG_H
#define _LINUX_SCSI_SG_H

#define SG_IO 0x2285

#define SG_DXFER_NONE (-1)
#define SG_DXFER_FROM_DEV (-3)

#define SG_FLAG_DIRECT_IO 1
#define SG_FLAG_LUN_INHIBIT 2

#define SG_INFO_OK 0x0
#define SG_INFO_OK_MASK 0x1

#ifdef __cplusplus
extern "C" {
#endif

struct sg_io_hdr {
    int interface_id;
    int dxfer_direction;
    unsigned char cmd_len;
    unsigned char mx_sb_len;
    unsigned short iovec_count;
    unsigned int dxfer_len;
    void *dxferp;
    unsigned char *cmdp;
    void *sbp;
    unsigned int timeout;
    unsigned int flags;
    int pack_id;
    void *usr_ptr;
    unsigned char status;
    unsigned char masked_status;
    unsigned char msg_status;
    unsigned char sb_len_wr;
    unsigned short host_status;
    unsigned short driver_status;
    int resid;
    unsigned int duration;
    unsigned int info;
};

#ifdef __cplusplus
}
#endif

#endif // _LINUX_SCSI_SG_H

