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
	long sp_lstchg;
	long sp_min;
	long sp_max;
	long sp_warn;
	long sp_inact;
	long sp_expire;
	unsigned long sp_flag;
};

#define SHADOW _PATH_SHADOW

#ifndef __MLIBC_ABI_ONLY

int putspent(const struct spwd *__sp, FILE *__f);
int lckpwdf(void);
int ulckpwdf(void);
struct spwd *getspnam(const char *__name);
int getspnam_r(const char *__name, struct spwd *__sp, char *__buf, size_t __size, struct spwd **__res);
struct spwd *fgetspent(FILE *__f);
void endspent(void);
struct spwd *sgetspent(const char *__s);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif
