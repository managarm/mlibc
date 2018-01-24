
#include <mntent.h>
#include <bits/ensure.h>

FILE *setmntent(const char *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct mntent *getmntent(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int addmntent(FILE *, const struct mntent *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int endmntent(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *hasmntopt(const struct mntent *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct mntent *getmntent_r(FILE *, struct mntent *,  char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

