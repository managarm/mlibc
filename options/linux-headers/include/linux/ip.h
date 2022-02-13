#ifndef _LINUX_IP_H
#define _LINUX_IP_H

#include <stdint.h>

struct iphdr {
	uint8_t ihl:4, version:4;
	uint8_t tos;
	uint16_t tot_len;
	uint16_t id;
	uint16_t frag_off;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t check;
	uint32_t saddr;
	uint32_t daddr;
};

#endif
