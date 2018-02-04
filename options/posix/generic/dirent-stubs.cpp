
#include <dirent.h>
#include <unistd.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/sysdeps.hpp>

int alphasort(const struct dirent **, const struct dirent **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int closedir(DIR *dir) {
	// TODO: Deallocate the dir structure.
	close(dir->__handle);
	return 0;
}
int dirfd(DIR *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
DIR *fdopendir(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
DIR *opendir(const char *path) {
	auto dir = frigg::construct<__mlibc_dir_struct>(getAllocator());
	__ensure(dir);
	dir->__ent_next = 0;
	dir->__ent_limit = 0;

	if(mlibc::sys_open_dir(path, &dir->__handle)) {
		frigg::destruct(getAllocator(), dir);
		return nullptr;
	}else{
		return dir;
	}
}
struct dirent *readdir(DIR *dir) {
	__ensure(dir->__ent_next <= dir->__ent_limit);
	if(dir->__ent_next == dir->__ent_limit) {
		if(mlibc::sys_read_entries(dir->__handle, dir->__ent_buffer, 2048, &dir->__ent_limit))
			__ensure(!"mlibc::sys_read_entries() failed");
		dir->__ent_next = 0;
		if(!dir->__ent_limit)
			return nullptr;
	}

	auto entp = reinterpret_cast<struct dirent *>(dir->__ent_buffer + dir->__ent_next);
	memcpy(&dir->__current, entp, sizeof(struct dirent));
	dir->__ent_next += entp->d_reclen;
	return &dir->__current;
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

