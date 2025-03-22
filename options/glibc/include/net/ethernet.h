#ifndef _NET_ETHERNET_H
#define _NET_ETHERNET_H

#include <bits/ether_addr.h>
#include <stdint.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __MLIBC_LINUX_OPTION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#pragma GCC diagnostic ignored "-Wpedantic"
#	include <linux/if_ether.h>
#pragma GCC diagnostic pop
#endif /* __MLIBC_LINUX_OPTION */

#define ETHERTYPE_PUP 0x0200
#define ETHERTYPE_SPRITE 0x0500
#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_REVARP 0x8035
#define ETHERTYPE_AT 0x809B
#define ETHERTYPE_AARP 0x80F3
#define ETHERTYPE_VLAN 0x8100
#define ETHERTYPE_IPX 0x8137
#define ETHERTYPE_IPV6 0x86dd
#define ETHERTYPE_LOOPBACK 0x9000

struct ether_header {
	uint8_t ether_dhost[6];
	uint8_t ether_shost[6];
	uint16_t ether_type;
};

#define ETHER_ADDR_LEN 6

#define ETHERTYPE_IP 0x0800

#ifdef __cplusplus
}
#endif

#endif
