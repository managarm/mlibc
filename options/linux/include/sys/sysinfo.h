#ifndef _SYS_SYSINFO_H
#define _SYS_SYSINFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: This is from the Linux ABI. Make this an abi-bit. */

struct sysinfo {
	long uptime;
	unsigned long loads[3];
	unsigned long totalram;
	unsigned long freeram;
	unsigned long sharedram;
	unsigned long bufferram;
	unsigned long totalswap;
	unsigned long freeswap;
	unsigned short procs;
	unsigned long totalhigh;
	unsigned long freehigh;
	unsigned int mem_unit;

	/* This is how the kernel header defines it, so suppress the warning. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	char _f[20 - 2 * sizeof(long) - sizeof(int)]; /* Padding to 64 bytes according to my man page */
#pragma GCC diagnostic pop
};

#define SI_LOAD_SHIFT 16

#ifndef __MLIBC_ABI_ONLY

int sysinfo(struct sysinfo *__info);

int get_nprocs(void);
int get_nprocs_conf(void);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SYSINFO_H */
