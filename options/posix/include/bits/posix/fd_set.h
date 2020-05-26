
#ifndef MLIBC_FD_SET_H
#define MLIBC_FD_SET_H

// FIXME: use something like uint8_t with fixed bit-size
typedef struct {
	union {
		char __mlibc_elems[128];
		// Some programs require the fds_bits field to be present
		char fds_bits[128];
	};
} fd_set;

#endif // MLIBC_FD_SET_H
