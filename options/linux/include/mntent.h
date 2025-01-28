#ifndef _MNTENT_H
#define _MNTENT_H

#include <stdio.h>

/* TODO: Refer to _PATH_MOUNTED */
#define MOUNTED "/etc/mtab"

/* Generic mount options */
#define MNTOPT_DEFAULTS "defaults"		/* Use all default options. */
#define MNTOPT_RO       "ro"			/* Read only. */
#define MNTOPT_RW       "rw"			/* Read/write. */
#define MNTOPT_SUID     "suid"			/* Set uid allowed. */
#define MNTOPT_NOSUID   "nosuid"		/* No set uid allowed. */
#define MNTOPT_NOAUTO   "noauto"		/* Do not auto mount. */

#ifdef __cplusplus
extern "C" {
#endif

struct mntent {
	char *mnt_fsname;
	char *mnt_dir;
	char *mnt_type;
	char *mnt_opts;
	int mnt_freq;
	int mnt_passno;
};

#ifndef __MLIBC_ABI_ONLY

FILE *setmntent(const char *__filename, const char *__type);

struct mntent *getmntent(FILE *__f);

int addmntent(FILE *__f, const struct mntent *__mnt);

int endmntent(FILE *__f);

char *hasmntopt(const struct mntent *__mnt, const char *__opt);

struct mntent *getmntent_r(FILE *__f, struct mntent *__mnt, char *__linebuf, int __buflen);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MNTENT_H */
