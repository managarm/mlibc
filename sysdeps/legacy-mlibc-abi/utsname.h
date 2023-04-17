#ifndef _ABIBITS_UTSNAME_T_H
#define _ABIBITS_UTSNAME_T_H

#if __MLIBC_BUILDING_MLIBC
#warning abis/mlibc/utsname.h is deprecated. We suggest to use abis/linux/utsname.h instead. \
	Note that this will potentially require kernel changes.
#endif

struct utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
};

#endif // _ABIBITS_UTSNAME_T_H
