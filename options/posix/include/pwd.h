
#ifndef _PWD_H
#define _PWD_H

#include <mlibc-config.h>

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <bits/size_t.h>

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

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
void endpwent(void);
struct passwd *getpwent(void);
void setpwent(void);
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN */

struct passwd *getpwnam(const char *__name);
int getpwnam_r(const char *__name, struct passwd *__ret, char *__buf, size_t __buflen, struct passwd **__res);
struct passwd *getpwuid(uid_t __uid);
int getpwuid_r(uid_t __uid, struct passwd *__ret, char *__buf, size_t __buflen, struct passwd **__res);

#if defined(_DEFAULT_SOURCE)
#include <bits/file.h>

int putpwent(const struct passwd *__pwd, FILE *__f);
struct passwd *fgetpwent(FILE *__f);
#endif /* defined(_DEFAULT_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _PWD_H */

