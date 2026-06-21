
#include <errno.h>
#include <mntent.h>

FILE *setmntent(const char *, const char *) {
	errno = ENOSYS;
	return nullptr;
}

struct mntent *getmntent(FILE *) {
	errno = ENOSYS;
	return nullptr;
}

int addmntent(FILE *, const struct mntent *) {
	errno = ENOSYS;
	return 1;
}

int endmntent(FILE *) {
	errno = ENOSYS;
	return 1;
}

char *hasmntopt(const struct mntent *, const char *) {
	errno = ENOSYS;
	return nullptr;
}

struct mntent *getmntent_r(FILE *, struct mntent *, char *, int) {
	errno = ENOSYS;
	return nullptr;
}
