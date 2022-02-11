
#ifndef _LINUX_CDROM_H
#define _LINUX_CDROM_H

#include <linux/types.h>
#include <stdint.h>

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

#define CDS_NO_INFO 0
#define CDS_NO_DISC 1
#define CDS_TRAY_OPEN 2
#define CDS_DRIVE_NOT_READY 3
#define CDS_DISC_OK 4

/* The generic packet command opcodes for CD/DVD Logical Units,
 * From Table 57 of the SFF8090 Ver. 3 (Mt. Fuji) draft standard. */
#define GPCMD_GET_CONFIGURATION 0x46
#define GPCMD_INQUIRY 0x12
#define GPCMD_READ_10 0x28
#define GPCMD_READ_DISC_INFO 0x51
#define GPCMD_READ_DVD_STRUCTURE 0xad
#define GPCMD_READ_FORMAT_CAPACITIES 0x23
#define GPCMD_READ_TOC_PMA_ATIP 0x43
#define GPCMD_START_STOP_UNIT 0x1b

#ifdef __cplusplus
extern "C" {
#endif

struct request_sense {
	uint8_t error_code : 7;
	uint8_t valid : 1;

	uint8_t segment_number;
	uint8_t sense_key : 4;
	uint8_t reserved2 : 1;
	uint8_t ili : 1;
	uint8_t reserved1 : 2;

	uint8_t information[4];
	uint8_t add_sense_len;
	uint8_t command_info[4];
	uint8_t asc;
	uint8_t ascq;
	uint8_t fruc;
	uint8_t sks[3];
	uint8_t asb[46];
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

typedef struct {
	uint16_t disc_information_length;

	uint8_t disc_status                : 2;
	uint8_t border_status              : 2;
	uint8_t erasable                   : 1;
	uint8_t reserved1                  : 3;

	uint8_t n_first_track;
	uint8_t n_sessions_lsb;
	uint8_t first_track_lsb;
	uint8_t last_track_lsb;

	uint8_t mrw_status                 : 2;
	uint8_t dbit                       : 1;
	uint8_t reserved2                  : 2;
	uint8_t uru                        : 1;
	uint8_t dbc_v                      : 1;
	uint8_t did_v                      : 1;

	uint8_t disc_type;
	uint8_t n_sessions_msb;
	uint8_t first_track_msb;
	uint8_t last_track_msb;
	uint32_t disc_id;
	uint32_t lead_in;
	uint32_t lead_out;
	uint8_t disc_bar_code[8];
	uint8_t reserved3;
	uint8_t n_opc;
} disc_information;

#ifdef __cplusplus
}
#endif

#endif // _LINUX_CDROM_H

