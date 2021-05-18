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

	s = getpwuid_r(0, &pwd, buf, bufsize, &result);
	assert(result);
	assert(pwd.pw_uid == 0);
	assert(!strcmp(pwd.pw_name, "root"));
	
	result = getpwnam("root");
	assert(result);
	assert(pwd.pw_uid == 0);
	assert(!strcmp(pwd.pw_name, "root"));

	result = getpwuid(0);
	assert(result);
	assert(pwd.pw_uid == 0);
	assert(!strcmp(pwd.pw_name, "root"));
}
