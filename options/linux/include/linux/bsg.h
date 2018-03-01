
#ifndef _LINUX_BSG_H
#define _LINUX_BSG_H

#define BSG_PROTOCOL_SCSI 0
#define BSG_SUB_PROTOCOL_SCSI_CMD 0
#define BSG_SUB_PROTOCOL_SCSI_TMF 1
#define BSG_SUB_PROTOCOL_SCSI_TRANSPORT 2

#ifdef __cplusplus
extern "C" {
#endif

struct sg_io_v4 {
	int32_t guard;
	uint32_t protocol;
	uint32_t subprotocol;

	uint32_t request_len;
	uint64_t request;
	uint64_t request_tag;
	uint32_t request_attr;
	uint32_t request_priority;
	uint32_t request_extra;
	uint32_t max_response_len;
	uint64_t response;

	uint32_t dout_iovec_count;
	uint32_t dout_xfer_len;
	uint32_t din_iovec_count;
	uint32_t din_xfer_len;
	uint64_t dout_xferp;
	uint64_t din_xferp;

	uint32_t timeout;
	uint32_t flags;
	uint64_t usr_ptr;
	uint32_t spare_in;

	uint32_t driver_status;
	uint32_t transport_status;
	uint32_t device_status;
	uint32_t retry_delay;
	uint32_t info;
	uint32_t duration;
	uint32_t response_len;
	int32_t din_resid;
	int32_t dout_resid;
	uint64_t generated_tag;
	uint32_t spare_out;

	uint32_t padding;
};

#ifdef __cplusplus
}
#endif

#endif // _LINUX_BSG_H

