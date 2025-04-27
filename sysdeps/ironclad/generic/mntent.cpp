#include <errno.h>
#include <mntent.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <bits/ensure.h>

namespace {

char *internal_buf;
size_t internal_bufsize;

}

#define SENTINEL (char *)&internal_buf

FILE *setmntent(const char *name, const char *mode) {
	return fopen(name, mode);
}

struct mntent *getmntent(FILE *f) {
	static struct mntent mnt;
	return getmntent_r(f, &mnt, SENTINEL, 0);
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

/* Adapted from musl */
struct mntent *getmntent_r(FILE *f, struct mntent *mnt, char *linebuf, int buflen) {
	bool use_internal = (linebuf == SENTINEL);

	char source[60];
	char target[30];
	char fs[30];
	char options[30];
	int dump;
	int pass;

	do {
		if (use_internal) {
			getline(&internal_buf, &internal_bufsize, f);
			linebuf = internal_buf;
		} else {
			fgets(linebuf, buflen, f);
		}

		if (feof(f) || ferror(f)) {
			return 0;
		}

		if (sscanf(linebuf, "%s %s %s %s %d %d\n", source, target, fs, options,
		           &dump, &pass) != 6) {
			continue;
		}
	} while(linebuf[0] == '#');

	mnt->mnt_fsname = strdup(source);
	mnt->mnt_dir    = strdup(target);
	mnt->mnt_type   = strdup(fs);
	mnt->mnt_opts   = strdup(options);
	mnt->mnt_freq   = dump;
	mnt->mnt_passno = pass;
	return mnt;
}
