#ifndef _MLIBC_INTERNAL_ETHER_ADDR_H
#define _MLIBC_INTERNAL_ETHER_ADDR_H

#include <stdint.h>

struct ether_addr {
	uint8_t ether_addr_octet[6];
} __attribute__((__packed__));

#endif /* _MLIBC_INTERNAL_ETHER_ADDR_H */
