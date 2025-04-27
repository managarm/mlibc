#ifndef _ABIBITS_RLIM_T_H
#define _ABIBITS_RLIM_T_H

#if __INTPTR_WIDTH__ == 32
typedef unsigned long long int rlim_t;
#else
typedef unsigned long int rlim_t;
#endif

#endif /* _ABIBITS_RLIM_T_H */
