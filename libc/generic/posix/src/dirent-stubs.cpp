
#include <dirent.h>

#include <mlibc/ensure.h>

int alphasort(const struct dirent **, const struct dirent **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int closedir(DIR *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int dirfd(DIR *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
DIR *fdopendir(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
DIR *opendir(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct dirent *readdir(DIR *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int readdir_r(DIR *__restrict, struct dirent *__restrict, struct dirent **__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void rewinddir(DIR *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *),
		int (*)(const struct dirent **, const struct dirent **)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void seekdir(DIR *, long) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long telldir(DIR *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

