#ifndef MLIBC_ETHER_ADDR_H
#define MLIBC_ETHER_ADDR_H

#include <stdint.h>

struct ether_addr {
	uint8_t ether_addr_octet[6];
} __attribute__((__packed__));

#endif /* MLIBC_ETHER_ADDR_H */
