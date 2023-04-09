#ifndef _SHADOW_H
#define _SHADOW_H

#include <stdint.h>
#include <stdio.h>
#include <paths.h>

#ifdef __cplusplus
extern "C" {
#endif

struct spwd {
	char *sp_namp;
	char *sp_pwdp;
	int32_t sp_lstchg;
	int32_t sp_min;
	int32_t sp_max;
	int32_t sp_warn;
	int32_t sp_inact;
	int32_t sp_expire;
	uint32_t sp_flag;
};

#define SHADOW _PATH_SHADOW

#ifndef __MLIBC_ABI_ONLY

int putspent(const struct spwd *, FILE *);
int lckpwdf(void);
int ulckpwdf(void);
struct spwd *getspnam(const char *);
int getspnam_r(const char *, struct spwd *, char *, size_t, struct spwd **);
struct spwd *fgetspent(FILE *);
void endspent(void);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif
