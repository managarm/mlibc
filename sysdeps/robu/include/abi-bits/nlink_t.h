
#ifndef _ABIBITS_NLINK_T_H
#define _ABIBITS_NLINK_T_H

#if defined(__x86_64__)
typedef unsigned long nlink_t;
#else
typedef unsigned int nlink_t;
#endif

#endif /* _ABIBITS_NLINK_T_H */

