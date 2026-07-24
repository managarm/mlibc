#ifndef _ABIBITS_UTSNAME_H
#define _ABIBITS_UTSNAME_H

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};

#endif /* _ABIBITS_UTSNAME_H */
