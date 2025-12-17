#ifndef _ABIBITS_SIGSET_T_H
#define _ABIBITS_SIGSET_T_H

typedef struct {
	unsigned long __sig[64 / (8 * sizeof(long))];
} sigset_t;

#endif /* _ABIBITS_SIGSET_T_H */
