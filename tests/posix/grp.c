#include <stdlib.h>
#include <stdio.h>
#include <grp.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

void check_root_group(struct group *grp) {
	assert(grp);

	printf("group name: %s\n", grp->gr_name);
	fflush(stdout);

	assert(grp->gr_gid == 0);
	assert(!strcmp(grp->gr_name, "root"));

	// Depending on your system, the root group may or may not contain the root user.
	if (grp->gr_mem[0] != NULL) {
		bool found = false;
		for (size_t i = 0; grp->gr_mem[i] != NULL; i++) {
			printf("group member: %s\n", grp->gr_mem[i]);
			fflush(stdout);

			if (!strcmp(grp->gr_mem[i], "root")) {
				found = true;
				break;
			}
		}
		assert(found);
	}
}

int main()
{
	struct group grp, *result = NULL;
	char *buf;
	size_t bufsize;
	int s;

	bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
	assert(bufsize > 0 && bufsize < 0x100000);

	buf = malloc(bufsize);
	assert(buf);

	s = getgrnam_r("root", &grp, buf, bufsize, &result);
	assert(!s);
	check_root_group(result);

	s = getgrgid_r(0, &grp, buf, bufsize, &result);
	assert(!s);
	check_root_group(result);

	result = getgrnam("root");
	check_root_group(result);

	result = getgrgid(0);
	check_root_group(result);

	result = getgrnam("this_group_doesnt_exist");
	assert(!result);
	assert(errno == ESRCH);
	s = getgrnam_r("this_group_doesnt_exist", &grp, buf, bufsize, &result);
	assert(!result);
	assert(s == ESRCH);
}
