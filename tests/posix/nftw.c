#include <assert.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static int files_visited = 0;
static int dirs_visited = 0;

static int count_cb(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
	(void)fpath;
	(void)sb;
	(void)ftwbuf;

	if (tflag == FTW_F) {
		files_visited++;
	} else if (tflag == FTW_D || tflag == FTW_DP) {
		dirs_visited++;
	}
	return 0;
}

static int rm_cb(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
	(void)sb;
	(void)tflag;
	(void)ftwbuf;
	return remove(fpath);
}

int main(void) {
	char tmpl[] = "/tmp/nftw_test_XXXXXX";
	char *test_dir = mkdtemp(tmpl);
	assert(test_dir != NULL);

	char path[256];
	int w = snprintf(path, sizeof(path), "%s/file1.txt", test_dir);
	assert(w > 0 && (size_t)w < sizeof(path));
	int fd = creat(path, 0666);
	assert(fd != -1);
	close(fd);

	w = snprintf(path, sizeof(path), "%s/sub", test_dir);
	assert(w > 0 && (size_t)w < sizeof(path));
	assert(mkdir(path, 0777) == 0);

	w = snprintf(path, sizeof(path), "%s/sub/file2.txt", test_dir);
	assert(w > 0 && (size_t)w < sizeof(path));
	fd = creat(path, 0666);
	assert(fd != -1);
	close(fd);

	int ret = nftw(test_dir, count_cb, 10, FTW_PHYS);
	assert(ret == 0);

	assert(files_visited == 2);
	assert(dirs_visited == 2);

	ret = nftw(test_dir, rm_cb, 10, FTW_DEPTH | FTW_PHYS);
	assert(ret == 0);

	struct stat st;
	assert(stat(test_dir, &st) == -1);

	return 0;
}
