
#ifndef _LINUX_CDROM_H
#define _LINUX_CDROM_H

#include <linux/types.h>

#define CDROM_PACKET_SIZE 12

#define CDO_LOCK 0x8

#define CDROM_CLEAR_OPTIONS 0x5321
#define CDROM_DRIVE_STATUS 0x5326
#define CDROM_LOCKDOOR 0x5329
#define CDROM_GET_CAPABILITY 0x5331

#define CDC_CD_R 0x2000
#define CDC_CD_RW 0x4000
#define CDC_DVD 0x8000
#define CDC_DVD_R 0x10000
#define CDC_DVD_RAM 0x20000
#define CDC_MRW 0x80000
#define CDC_MRW_W 0x100000

#define CDSL_CURRENT INT_MAX

#define CDS_DISC_OK 4

#ifdef __cplusplus
extern "C" {
#endif

struct request_sense {
//#if defined(__BIG_ENDIAN_BITFIELD)
//	__u8 valid : 1;
//	__u8 error_code : 7;
//#elif defined(__LITTLE_ENDIAN_BITFIELD)
	__u8 error_code : 7;
	__u8 valid : 1;
//#endif
	__u8 segment_number;
//#if defined(__BIG_ENDIAN_BITFIELD)
//	__u8 reserved1 : 2;
//	__u8 ili : 1;
//	__u8 reserved2 : 1;
//	__u8 sense_key : 4;
//#elif defined(__LITTLE_ENDIAN_BITFIELD)
	__u8 sense_key : 4;
	__u8 reserved2 : 1;
	__u8 ili : 1;
	__u8 reserved1 : 2;
//#endif
	__u8 information[4];
	__u8 add_sense_len;
	__u8 command_info[4];
	__u8 asc;
	__u8 ascq;
	__u8 fruc;
	__u8 sks[3];
	__u8 asb[46];
};

struct cdrom_generic_command {
	unsigned char cmd[CDROM_PACKET_SIZE];
	unsigned char *buffer;
	unsigned int buflen;
	int	stat;
	struct request_sense *sense;
	unsigned char data_direction;
	int	quiet;
	int	timeout;
	void *reserved[1];
};

#ifdef __cplusplus
}
#endif

#endif // _LINUX_CDROM_H

