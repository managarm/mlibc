#include <assert.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
	int ngroups = 100;
	struct passwd *pw;
	struct group *gr;
	gid_t *groups;
	bool gid_checked = false;
	int e = 0;

	groups = malloc(sizeof(*groups) * ngroups);
	if(groups == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	/* Fetch passwd structure */
	pw = getpwuid(geteuid());
	if(pw == NULL) {
		perror("getpwnam");
		exit(EXIT_FAILURE);
	}

	/* Retrieve group list */
	e = getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups);
	assert(e > 0);
	if(e == -1) {
		fprintf(stderr, "getgrouplist() returned -1 (%s); ngroups = %d\n", strerror(errno), ngroups);
		exit(EXIT_FAILURE);
	}

	/* Display list of retrieved groups, along with group names */
	fprintf(stderr, "ngroups = %d\n", ngroups);
	for(int j = 0; j < ngroups; j++) {
		if(groups[j] == pw->pw_gid)
			gid_checked = true;
		printf("%d", groups[j]);
		gr = getgrgid(groups[j]);
		if(gr != NULL)
			printf(" (%s)", gr->gr_name);
		printf("\n");
	}

	assert(gid_checked);

	exit(EXIT_SUCCESS);
}
