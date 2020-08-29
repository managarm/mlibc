
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/file-io.hpp>

FILE *fmemopen(void *__restrict, size_t, const char *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pclose(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

FILE *popen(const char*, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

FILE *open_memstream(char **, size_t *) {
	mlibc::infoLogger() << "\e[31mmlibc: open_memstream() always fails"
			<< "\e[39m" << frg::endlog;
	errno = ENOMEM;
	return nullptr;
}

int fseeko(FILE *file_base, off_t offset, int whence) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(int e = file->seek(offset, whence); e) {
		errno = e;
		return -1;
	}
	return 0;
}

off_t ftello(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	off_t current_offset;
	if(int e = file->tell(&current_offset); e) {
		errno = e;
		return -1;
	}
	return current_offset;
}

int dprintf(int fd, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vdprintf(fd, format, args);
    va_end(args);
    return result;
}

int vdprintf(int fd, const char *format, __gnuc_va_list args) {
    FILE *file = fdopen(fd, "a");
    int ret = vfprintf(file, format, args);
    fclose(file);
    return ret;
}

