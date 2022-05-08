#ifndef _LINUX_WIRELESS_H
#define _LINUX_WIRELESS_H

#include <sys/types.h>
#include <linux/socket.h>
#include <linux/if.h>

#include <stddef.h>

#define IW_ESSID_MAX_SIZE 32

#define SIOCGIWESSID 0x8B1B

struct iw_param {
	int32_t value;
	uint8_t fixed;
	uint8_t disabled;
	uint16_t flags;
};

struct iw_point {
	void *pointer;
	uint16_t length;
	uint16_t flags;
};

struct iw_freq {
	int32_t m;
	int16_t e;
	uint8_t i;
	uint8_t flags;
};

struct iw_quality {
	uint8_t qual;
	uint8_t level;
	uint8_t noise;
	uint8_t updated;
};

union iwreq_data {
	char name[IFNAMSIZ];

	struct iw_point	essid;
	struct iw_param	nwid;
	struct iw_freq	freq;

	struct iw_param	sens;
	struct iw_param	bitrate;
	struct iw_param	txpower;
	struct iw_param	rts;
	struct iw_param	frag;
	uint32_t mode;
	struct iw_param	retry;

	struct iw_point	encoding;
	struct iw_param	power;
	struct iw_quality qual;

	struct sockaddr	ap_addr;
	struct sockaddr	addr;

	struct iw_param	param;
	struct iw_point	data;
};

struct iwreq {
	union {
		char ifrn_name[IFNAMSIZ];
	} ifr_ifrn;

	union iwreq_data u;
};

#endif
