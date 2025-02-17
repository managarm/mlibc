#ifndef _SYS_MTIO_H
#define _SYS_MTIO_H

#include <mlibc-config.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mtop {
	short int mt_op;
	int mt_count;
};

struct mtget {
	long int mt_type;
	long int mt_resid;
	long int mt_dsreg;
	long int mt_gstat;
	long int mt_erreg;
	int mt_fileno;
	int mt_blkno;
};

struct mtpos {
	long int mt_blkno;
};

struct mtconfiginfo {
	long int mt_type;
	long int ifc_type;
	unsigned short int irqnr;
	unsigned short int dmanr;
	unsigned short int port;

	unsigned long int debug;

	unsigned have_dens:1;
	unsigned have_bsf:1;
	unsigned have_fsr:1;
	unsigned have_bsr:1;
	unsigned have_eod:1;
	unsigned have_seek:1;
	unsigned have_tell:1;
	unsigned have_ras1:1;
	unsigned have_ras2:1;
	unsigned have_ras3:1;
	unsigned have_qfa:1;

	unsigned pad1:5;
	char reserved[10];
};

#define MTRESET 0
#define MTFSF 1
#define MTBSF 2
#define MTFSR 3
#define MTBSR 4
#define MTWEOF 5
#define MTREW 6
#define MTOFFL 7
#define MTNOP 8
#define MTRETEN 9
#define MTBSFM 10
#define MTFSFM 11
#define MTEOM 12
#define MTERASE 13
#define MTRAS1 14
#define MTRAS2 15
#define MTRAS3 16
#define MTSETBLK 20
#define MTSETDENSITY 21
#define MTSEEK 22
#define MTTELL 23
#define MTSETDRVBUFFER 24
#define MTFSS 25
#define MTBSS 26
#define MTWSM 27
#define MTLOCK 28
#define MTUNLOCK 29
#define MTLOAD 30
#define MTUNLOAD 31
#define MTCOMPRESSION 32
#define MTSETPART 33
#define MTMKPART 34

#define GMT_WR_PROT(x) ((x) & 0x04000000)

#if __MLIBC_LINUX_OPTION
#define MTIOCTOP _IOR('m', 1, struct mtop)
#define MTIOCGET _IOR('m', 2, struct mtget)
#define MTIOCPOS _IOR('m', 3, struct mtpos)
#define MTIOCGETCONFIG _IOR('m', 4, struct mtconfiginfo)
#define MTIOCSETCONFIG _IOR('m', 5, struct mtconfiginfo)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MTIO_H */
