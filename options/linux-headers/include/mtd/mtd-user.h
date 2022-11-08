#ifndef _MTD_USER_H
#define _MTD_USER_H

#include <stdint.h>
#include <sys/ioctl.h>

#define MTD_NANDFLASH 4

struct mtd_info_user {
	uint8_t type;
	uint32_t flags;
	uint32_t size;
	uint32_t erasesize;
	uint32_t writesize;
	uint32_t oobsize;
	uint64_t padding;
};

#define MEMGETINFO _IOR('M', 1, struct mtd_info_user)

typedef struct mtd_info_user mtd_info_t;

#endif // _MTD_USER_H
