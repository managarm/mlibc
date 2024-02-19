#ifndef MLIBC_FD_SET_H
#define MLIBC_FD_SET_H

typedef struct {
	union {
		__UINT8_TYPE__ __mlibc_elems[128];
		// Some programs require the fds_bits field to be present
		__UINT8_TYPE__ fds_bits[128];
	};
} fd_set;

#endif // MLIBC_FD_SET_H
