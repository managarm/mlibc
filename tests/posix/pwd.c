#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

int main()
{
	struct passwd pwd, *result = NULL;
	char *buf;
	size_t bufsize;
	int s;

	bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
	assert(bufsize > 0 && bufsize < 0x100000);

	buf = malloc(bufsize);
	assert(buf);

	s = getpwnam_r("root", &pwd, buf, bufsize, &result);
	assert(!s);
	assert(pwd.pw_uid == 0);
	assert(!strcmp(pwd.pw_name, "root"));
	assert(strlen(pwd.pw_passwd) <= 1000);

	s = getpwuid_r(0, &pwd, buf, bufsize, &result);
	assert(!s);
	assert(pwd.pw_uid == 0);
	assert(!strcmp(pwd.pw_name, "root"));
	assert(strlen(pwd.pw_passwd) <= 1000);

	result = getpwnam("root");
	assert(result);
	assert(result->pw_uid == 0);
	assert(!strcmp(result->pw_name, "root"));
	assert(strlen(result->pw_passwd) <= 1000);

	result = getpwuid(0);
	assert(result);
	assert(result->pw_uid == 0);
	assert(!strcmp(result->pw_name, "root"));
	assert(strlen(result->pw_passwd) <= 1000);

	errno = 0;
	setpwent();
	assert(errno == 0);

	errno = 0;
	result = getpwent();
	assert(result);

	pwd = *result;
	pwd.pw_name = strdup(result->pw_name);
	pwd.pw_passwd = strdup(result->pw_passwd);
	pwd.pw_gecos = strdup(result->pw_gecos);
	pwd.pw_dir = strdup(result->pw_dir);
	pwd.pw_shell = strdup(result->pw_shell);
	assert(pwd.pw_name);
	assert(pwd.pw_passwd);
	assert(pwd.pw_gecos);
	assert(pwd.pw_dir);
	assert(pwd.pw_shell);

	errno = 0;
	setpwent();
	assert(errno == 0);

	errno = 0;
	result = getpwent();
	assert(result);

	assert(!strcmp(pwd.pw_name, result->pw_name));
	assert(!strcmp(pwd.pw_passwd, result->pw_passwd));
	assert(!strcmp(pwd.pw_gecos, result->pw_gecos));
	assert(!strcmp(pwd.pw_dir, result->pw_dir));
	assert(!strcmp(pwd.pw_shell, result->pw_shell));
	assert(pwd.pw_uid == result->pw_uid);
	assert(pwd.pw_gid == result->pw_gid);

	free(pwd.pw_name);
	free(pwd.pw_passwd);
	free(pwd.pw_gecos);
	free(pwd.pw_dir);
	free(pwd.pw_shell);

	pwd.pw_name = "managarm";
	pwd.pw_passwd = "passwordhash";
	pwd.pw_uid = 1234;
	pwd.pw_gid = 12345;
	pwd.pw_gecos = "/etc/password test";
	pwd.pw_dir = "/home/managarm\n";
	pwd.pw_shell = "/usr/bin/bash";

	// tmpfile() cannot be used because its unimplemented in mlibc.
	char filename[] = "pwdXXXXXX";
	int tmpfd = mkstemp(filename);
	assert(tmpfd);
	FILE *tmp = fdopen(tmpfd, "w+");
	assert(tmp);

	assert(putpwent(NULL, tmp) < 0);
	assert(putpwent(&pwd, tmp) < 0);

	pwd.pw_dir = "/home/managarm:";
	assert(putpwent(&pwd, tmp) < 0);

	pwd.pw_dir = "/home/:managarm\n";
	assert(putpwent(&pwd, tmp) < 0);

	pwd.pw_dir = "/home/managarm";
	assert(putpwent(&pwd, tmp) == 0);
	rewind(tmp);

	char *expected = "managarm:passwordhash:1234:12345:/etc/password test:/home/managarm:/usr/bin/bash\n";
	size_t expectedlen = strlen(expected);

	size_t readsize = fread(buf, 1, bufsize, tmp);
	assert(readsize == expectedlen);
	assert(strncmp(expected, buf, expectedlen) == 0);

	fclose(tmp);
	unlink(filename);
	free(buf);

	return 0;
}
