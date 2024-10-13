
#ifndef _SYS_PARAM_H
#define _SYS_PARAM_H

#include <endian.h>
#include <limits.h>

#define NBBY CHAR_BIT
#define NGROUPS NGROUPS_MAX

/* Report the same value as Linux here. */
#define MAXNAMLEN 255
#define MAXPATHLEN 4096
#define MAXSYMLINKS 20
#define MAXHOSTNAMELEN HOST_NAME_MAX

#ifdef __cplusplus
extern "C" {
#endif

#undef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#undef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define howmany(x, y)  (((x) + ((y) - 1)) / (y))

#define roundup(x, y)  ((((x) + ((y) - 1)) / (y)) * (y))

#ifdef __cplusplus
}
#endif

#endif /* _SYS_PARAM_H */

