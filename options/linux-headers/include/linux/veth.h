#ifndef _LINUX_VETH_H
#define _LINUX_VETH_H

enum {
	VETH_INFO_UNSPEC,
	VETH_INFO_PEER,

	__VETH_INFO_MAX
#define VETH_INFO_MAX (__VETH_INFO_MAX - 1)
};

#endif
