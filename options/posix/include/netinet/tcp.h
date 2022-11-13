#ifndef _NETINET_TCP_H
#define _NETINET_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

// Define some macros using same ABI as Linux
#define TCP_NODELAY 1
#define TCP_MAXSEG 2
#define TCP_KEEPIDLE 4
#define TCP_KEEPINTVL 5
#define TCP_KEEPCNT 6
#define TCP_DEFER_ACCEPT 9
#define TCP_CONGESTION 13

#define SOL_TCP 6

#ifdef __cplusplus
}
#endif

#endif // _NETINET_TCP_H
