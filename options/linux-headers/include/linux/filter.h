
#ifndef  _FILTER_H
#define  _FILTER_H

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

#include <stdint.h>

#include <linux/bpf_common.h>

struct sock_filter {
	uint16_t code;
	uint8_t jt;
	uint8_t jf;
	uint32_t k;
};

struct sock_fprog {
	unsigned short len;
	struct sock_filter *filter;
};

#ifdef __cplusplus
}
#endif __cplusplus

#endif // _FILTER_H

