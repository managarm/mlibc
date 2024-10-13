#ifndef _SYS_SIGNALFD_H
#define _SYS_SIGNALFD_H

/* TODO: Define sigset separately and remove this include. */
#include <signal.h>
/* musl includes those. Restructure this so we do not need them? */
#include <stdint.h>
#include <fcntl.h>

#define SFD_CLOEXEC O_CLOEXEC
#define SFD_NONBLOCK O_NONBLOCK

#ifdef __cplusplus
extern "C" {
#endif

struct signalfd_siginfo {
	uint32_t  ssi_signo;
	int32_t   ssi_errno;
	int32_t   ssi_code;
	uint32_t  ssi_pid;
	uint32_t  ssi_uid;
	int32_t   ssi_fd;
	uint32_t  ssi_tid;
	uint32_t  ssi_band;
	uint32_t  ssi_overrun;
	uint32_t  ssi_trapno;
	int32_t   ssi_status;
	int32_t   ssi_int;
	uint64_t  ssi_ptr;
	uint64_t  ssi_utime;
	uint64_t  ssi_stime;
	uint64_t  ssi_addr;
	uint16_t  ssi_addr_lsb;
	uint8_t   pad[128-12*4-4*8-2];
};

#ifndef __MLIBC_ABI_ONLY

int signalfd(int __fd, const sigset_t *__mask, int __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SIGNALFD_H */
