#ifndef _MLIBC_INTERNAL_MBSTATE_H
#define _MLIBC_INTERNAL_MBSTATE_H

typedef struct __mlibc_mbstate {
	short __progress;
	short __shift;
	unsigned int __cpoint;
} mbstate_t;

#define __MLIBC_MBSTATE_INITIALIZER {0, 0, 0}

#endif /* _MLIBC_INTERNAL_MBSTATE_H */
