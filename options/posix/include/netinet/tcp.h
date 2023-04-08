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
#define TCP_FASTOPEN 23

#define TCP_ESTABLISHED 1
#define TCP_SYN_SENT 2
#define TCP_SYN_RECV 3
#define TCP_FIN_WAIT1 4
#define TCP_FIN_WAIT2 5
#define TCP_TIME_WAIT 6
#define TCP_CLOSE 7
#define TCP_CLOSE_WAIT 8
#define TCP_LAST_ACK 9
#define TCP_LISTEN 10
#define TCP_CLOSING 11

#define SOL_TCP 6

#ifdef __cplusplus
}
#endif

#endif // _NETINET_TCP_H
