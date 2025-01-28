#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

struct testcookie {
	bool read;
	bool write;
	bool seek;
	bool close;
};

ssize_t cookie_read(void *c, char *buf, size_t size) {
	(void) buf;
	struct testcookie *cookie = c;
	cookie->read = true;
	return size;
}

ssize_t cookie_write(void *c, const char *buf, size_t size) {
	(void) buf;
	struct testcookie *cookie = c;
	cookie->write = true;
	return size;
}

int cookie_seek(void *c, off64_t *offset, int whence) {
	(void) offset;
	(void) whence;
	struct testcookie *cookie = c;
	cookie->seek = true;
	return 0;
}

int cookie_close(void *c) {
	struct testcookie *cookie = c;
	cookie->close = true;
	return 0;
}

int main() {
	struct testcookie cookie = { false, false, false, false };

	cookie_io_functions_t funcs = {
		.read = cookie_read,
		.write = cookie_write,
		.seek = cookie_seek,
		.close = cookie_close,
	};

	FILE *stream = fopencookie(&cookie, "w+", funcs);
	assert(stream);

	unsigned char buf[1];
	int ret = fread(buf, 1, 1, stream);
	assert(ret == 1);
	ret = fwrite(buf, 1, 1, stream);
	assert(ret == 1);
	ret = fseek(stream, 0, SEEK_SET);
	assert(!ret);

	ret = fclose(stream);
	assert(!ret);

	assert(cookie.read && cookie.write && cookie.seek && cookie.close);

	return 0;
}
