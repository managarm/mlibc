#ifndef _NETINET_UDP_H
#define _NETINET_UDP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

__extension__ struct udphdr {
        __extension__ union {
                struct {
                        uint16_t uh_sport;
                        uint16_t uh_dport;
                        uint16_t uh_ulen;
                        uint16_t uh_sum;
                };
                struct {
                        uint16_t source;
                        uint16_t dest;
                        uint16_t len;
                        uint16_t check;
                };
        };
};

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_UDP_H */
