#ifndef _SYS_MTIO_H
#define _SYS_MTIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

#if defined(__linux__) || defined(__MLIBC_LINUX_OPTION)
#include <asm/ioctl.h>
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

#if defined(__linux__) || defined(__MLIBC_LINUX_OPTION)
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
