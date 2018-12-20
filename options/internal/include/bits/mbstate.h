#ifndef MLIBC_MBSTATE_H
#define MLIBC_MBSTATE_H

struct __mlibc_mbstate {
	short __progress;
	short __shift;
	unsigned int __cpoint;
};

#define __MLIBC_MBSTATE_INITIALIZER {0, 0, 0}

#endif // MLIBC_MBSTATE_H
