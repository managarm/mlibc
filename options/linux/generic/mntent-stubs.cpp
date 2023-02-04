
#include <mntent.h>
#include <stdio.h>
#include <string.h>
#include <bits/ensure.h>

FILE *setmntent(const char *name, const char *mode) {
	return fopen(name, mode);
}

struct mntent *getmntent(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int addmntent(FILE *f, const struct mntent *mnt) {
	if(fseek(f, 0, SEEK_END)) {
		return 1;
	}
	return fprintf(f, "%s\t%s\t%s\t%s\t%d\t%d\n",
		mnt->mnt_fsname, mnt->mnt_dir, mnt->mnt_type, mnt->mnt_opts,
		mnt->mnt_freq, mnt->mnt_passno) < 0;
}

int endmntent(FILE *f) {
	if(f) {
		fclose(f);
	}
	return 1;
}

char *hasmntopt(const struct mntent *mnt, const char *opt) {
	return strstr(mnt->mnt_opts, opt);
}

struct mntent *getmntent_r(FILE *, struct mntent *,  char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
