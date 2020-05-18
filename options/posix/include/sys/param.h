
#ifndef _SYS_PARAM_H
#define _SYS_PARAM_H

#include <endian.h>
#include <limits.h>

#define NGROUPS NGROUPS_MAX

// Report the same value as Linux here.
#define MAXPATHLEN 4096
#define HOST_NAME_MAX 64
#define MAXHOSTNAMELEN HOST_NAME_MAX

#ifdef __cplusplus
extern "C" {
#endif

#undef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#undef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#ifdef __cplusplus
}
#endif

#endif // _SYS_PARAM_H

