#ifndef _SYS_SYSINFO_H
#define _SYS_SYSINFO_H

#ifdef __cplusplus
extern "C" {
#endif

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
	char _f[20 - 2 * sizeof(long) - sizeof(int)]; // Padding to 64 bytes according to my man page
};

int sysinfo(struct sysinfo *);

#ifdef __cplusplus
}
#endif

#endif // _SYS_SYSINFO_H
