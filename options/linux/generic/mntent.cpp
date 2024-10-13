
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
	int n[8];
	bool use_internal = (linebuf == SENTINEL);
	int len;
	size_t i;

	mnt->mnt_freq = 0;
	mnt->mnt_passno = 0;

	do {
		if(use_internal) {
			getline(&internal_buf, &internal_bufsize, f);
			linebuf = internal_buf;
		} else {
			fgets(linebuf, buflen, f);
		}
		if(feof(f) || ferror(f)) {
			return 0;
		}
		if(!strchr(linebuf, '\n')) {
			fscanf(f, "%*[^\n]%*[\n]");
			errno = ERANGE;
			return 0;
		}

		len = strlen(linebuf);
		if(len > INT_MAX) {
			continue;
		}

		for(i = 0; i < sizeof n / sizeof *n; i++) {
			n[i] = len;
		}

		sscanf(linebuf, " %n%*s%n %n%*s%n %n%*s%n %n%*s%n %d %d",
			n, n + 1, n + 2, n + 3, n + 4, n + 5, n + 6, n + 7,
			&mnt->mnt_freq, &mnt->mnt_passno);
	} while(linebuf[n[0]] == '#' || n[1] == len);

	linebuf[n[1]] = 0;
	linebuf[n[3]] = 0;
	linebuf[n[5]] = 0;
	linebuf[n[7]] = 0;

	mnt->mnt_fsname = linebuf + n[0];
	mnt->mnt_dir = linebuf + n[2];
	mnt->mnt_type = linebuf + n[4];
	mnt->mnt_opts = linebuf + n[6];

	return mnt;
}
