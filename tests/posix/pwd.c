#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

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
	assert(result);
	assert(pwd.pw_uid == 0);
	assert(!strcmp(pwd.pw_name, "root"));
	assert(strlen(pwd.pw_passwd) <= 1000);

	s = getpwuid_r(0, &pwd, buf, bufsize, &result);
	assert(result);
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
}
