#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ABIBITS_UTSNAME_H
#define _ABIBITS_UTSNAME_H

struct utsname {
    char sysname[128];
    char nodename[128];
    char release[128];
    char version[128];
    char machine[128];
};

#endif

#ifdef __cplusplus
}
#endif