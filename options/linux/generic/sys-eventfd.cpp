#include <sys/eventfd.h>

#include <bits/ensure.h>

int eventfd(unsigned int initval, int flags) {
	__ensure(!"not implemented");
}

int eventfd_read(int fd, eventfd_t *value) {
	__ensure(!"not implemented");
}

int eventfd_write(int fd, eventfd_t value) {
	__ensure(!"not implemented");
}
