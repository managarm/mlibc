#ifndef _PIGGY_ROUTE_H
#define _PIGGY_ROUTE_H

struct rtentry {
    char ifname[IFNAMSIZ];

    uint32_t destination;
    uint32_t gateway;
    uint32_t mask;

    uint32_t metric;
};

#endif /* _PIGGY_ROUTE_H */
