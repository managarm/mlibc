
#ifndef _PWD_H
#define _PWD_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <bits/size_t.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct passwd {
	char *pw_name;
	char *pw_passwd;
	uid_t pw_uid;
	gid_t pw_gid;
	char *pw_gecos;
	char *pw_dir;
	char *pw_shell;
};

#define NSS_BUFLEN_PASSWD 512

#ifndef __MLIBC_ABI_ONLY

void endpwent(void);
struct passwd *getpwent(void);
struct passwd *getpwnam(const char *);
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
struct passwd *getpwuid(uid_t);
int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
void setpwent(void);
int putpwent(const struct passwd *, FILE *);
struct passwd *fgetpwent(FILE *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _PWD_H

