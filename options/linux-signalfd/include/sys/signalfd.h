#ifndef _SYS_SIGNALFD_H
#define _SYS_SIGNALFD_H

#include <abi-bits/sigset_t.h>
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
	uint16_t  __pad2;
	int32_t   ssi_syscall;
	uint64_t  ssi_call_addr;
	uint32_t  ssi_arch;
	uint8_t   __pad[28];
};

#ifndef __MLIBC_ABI_ONLY

int signalfd(int __fd, const sigset_t *__mask, int __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SIGNALFD_H */
