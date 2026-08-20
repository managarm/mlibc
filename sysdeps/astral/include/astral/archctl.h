#ifndef _ASTRAL_ARCHCTL_H
#define _ASTRAL_ARCHCTL_H

#define ARCH_CTL_SET_GSBASE 0
#define ARCH_CTL_SET_FSBASE 1
#define ARCH_CTL_GET_GSBASE 2
#define ARCH_CTL_GET_FSBASE 3
#define ARCH_CTL_SET_SYSTRACE 4
#define ARCH_CTL_SET_LDT_ENTRY 5

#define ARCH_CTL_SYSTRACE_OFF 0
#define ARCH_CTL_SYSTRACE_SELF 1
#define ARCH_CTL_SYSTRACE_ALL 2

typedef struct {
	uint64_t entry;
	uint16_t seg;
} archctl_ldt_request_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int arch_ctl(int, void *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _ASTRAL_ARCH_CTLH */
