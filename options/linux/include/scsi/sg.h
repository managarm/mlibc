
#ifndef _LINUX_SCSI_SG_H
#define _LINUX_SCSI_SG_H

#define SG_IO 0x2285

#define SG_GET_VERSION_NUM 0x2282

#define SG_FLAG_DIRECT_IO 1
#define SG_FLAG_LUN_INHIBIT 2

#define SG_INFO_OK 0x0
#define SG_INFO_OK_MASK 0x1

#define SG_DXFER_NONE (-1)
#define SG_DXFER_TO_DEV (-2)
#define SG_DXFER_FROM_DEV (-3)
#define SG_DXFER_TO_FROM_DEV (-4)

#define SG_INFO_CHECK 0x1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sg_io_hdr {
    int interface_id;
    int dxfer_direction;
    unsigned char cmd_len;
    unsigned char mx_sb_len;
    unsigned short iovec_count;
    unsigned int dxfer_len;
    void *dxferp;
    unsigned char *cmdp;
    unsigned char *sbp;
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
} sg_io_hdr_t;

struct sg_scsi_id {
    int host_no;
    int channel;
    int scsi_id;
    int lun;
    int scsi_type;
    short int h_cmd_per_lun;
    short int d_queue_depth;
    int unused[2];
};

typedef struct sg_req_info {
    char req_state;
    char orphan;
    char sg_io_owned;
    char problem;
    int pack_id;
    void *usr_ptr;
    unsigned int duration;

    int unused;
} sg_req_info_t;

#ifdef __cplusplus
}
#endif

#endif /* _LINUX_SCSI_SG_H */

