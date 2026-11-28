#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>

static void test_access_modes(void) {
	int readable = __freadable(stdin);
	int writable = __fwritable(stdin);
	int reading = __freading(stdin);
	int writing = __fwriting(stdin);
	assert(readable != 0);
	assert(writable == 0);
	assert(reading != 0);
	assert(writing == 0);

	readable = __freadable(stderr);
	writable = __fwritable(stderr);
	reading = __freading(stderr);
	writing = __fwriting(stderr);
	assert(readable == 0);
	assert(writable != 0);
	assert(reading == 0);
	assert(writing != 0);

	FILE *f = fopen("/dev/null", "w");
	assert(f != NULL);
	readable = __freadable(f);
	writable = __fwritable(f);
	reading = __freading(f);
	writing = __fwriting(f);
	assert(readable == 0);
	assert(writable != 0);
	assert(reading == 0);
	assert(writing != 0);
	fclose(f);

	f = tmpfile();
	assert(f != NULL);
	readable = __freadable(f);
	writable = __fwritable(f);
	reading = __freading(f);
	writing = __fwriting(f);
	assert(readable != 0);
	assert(writable != 0);
	assert(reading == 0);
	assert(writing == 0);

	fputc('a', f);
	reading = __freading(f);
	writing = __fwriting(f);
	assert(reading == 0);
	assert(writing != 0);

	rewind(f);
	int c = fgetc(f);
	assert(c == 'a');
	reading = __freading(f);
	writing = __fwriting(f);
	assert(reading != 0);
	assert(writing == 0);
	fclose(f);

	f = tmpfile();
	assert(f != NULL);
	setvbuf(f, NULL, _IONBF, 0);
	fputc('a', f);
	reading = __freading(f);
	writing = __fwriting(f);
	assert(reading == 0);
	assert(writing != 0);

	rewind(f);
	c = fgetc(f);
	assert(c == 'a');
	reading = __freading(f);
	writing = __fwriting(f);
	assert(reading != 0);
	assert(writing == 0);
	fclose(f);
}

static void *try_lock_thread(void *arg) {
	FILE *f = arg;
	int ret = ftrylockfile(f);
	if(!ret)
		funlockfile(f);
	return (void *)(long)ret;
}

static int try_lock_from_other_thread(FILE *f) {
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, try_lock_thread, f);
	assert(ret == 0);
	void *result;
	ret = pthread_join(thread, &result);
	assert(ret == 0);
	return (int)(long)result;
}

static void test_locking(void) {
	FILE *f = tmpfile();
	assert(f != NULL);

	int state = __fsetlocking(f, FSETLOCKING_QUERY);
	assert(state == FSETLOCKING_INTERNAL);
	state = __fsetlocking(f, FSETLOCKING_BYCALLER);
	assert(state == FSETLOCKING_INTERNAL);
	state = __fsetlocking(f, FSETLOCKING_QUERY);
	assert(state == FSETLOCKING_BYCALLER);

	flockfile(f);
	int locked = try_lock_from_other_thread(f);
	assert(locked != 0);
	funlockfile(f);
	locked = try_lock_from_other_thread(f);
	assert(locked == 0);

	state = __fsetlocking(f, FSETLOCKING_INTERNAL);
	assert(state == FSETLOCKING_BYCALLER);

	flockfile(f);
	__fsetlocking(f, FSETLOCKING_BYCALLER);
	funlockfile(f);
	__fsetlocking(f, FSETLOCKING_INTERNAL);
	locked = try_lock_from_other_thread(f);
	assert(locked == 0);

	fclose(f);
}

static void test_flushlbf(void) {
	FILE *f = tmpfile();
	assert(f != NULL);
	setvbuf(f, NULL, _IOLBF, 0);

	fputs("abc", f);
	size_t pending = __fpending(f);
	assert(pending == 3);

	_flushlbf();
	pending = __fpending(f);
	assert(pending == 0);

	fclose(f);
}

int main() {
	test_access_modes();
	test_locking();
	test_flushlbf();

	FILE *f = fopen("/dev/zero", "r");
	assert(f != NULL);

	setvbuf(f, NULL, _IOLBF, 0);
	assert(__flbf(f) != 0);

	fgetc(f);
	assert(__fbufsize(f) > 0);

	setvbuf(f, NULL, _IOFBF, 0);
	assert(__flbf(f) == 0);

#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	// glibc does not support __fseterr
	assert(ferror(f) == 0);
	__fseterr(f);
	assert(ferror(f) != 0);
#endif

	fclose(f);

	// glibc does not support __freadptr
#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	static const char data[] = "hello, world";

	f = tmpfile();
	assert(f != NULL);
	size_t written = fwrite(data, 1, sizeof(data) - 1, f);
	assert(written == sizeof(data) - 1);
	rewind(f);

	size_t size = 0;
	const char *ptr = __freadptr(f, &size);
	assert(ptr == NULL);

	int c = fgetc(f);
	assert(c == 'h');

	ptr = __freadptr(f, &size);
	assert(ptr != NULL);
	assert(size == sizeof(data) - 2);
	int cmp = memcmp(ptr, data + 1, size);
	assert(cmp == 0);

	int pushed = ungetc('X', f);
	assert(pushed == 'X');
	ptr = __freadptr(f, &size);
	assert(ptr != NULL);
	assert(size == 1);
	assert(*ptr == 'X');

	c = fgetc(f);
	assert(c == 'X');
	ptr = __freadptr(f, &size);
	assert(ptr != NULL);
	assert(size == sizeof(data) - 2);
	cmp = memcmp(ptr, data + 1, size);
	assert(cmp == 0);

	__freadptrinc(f, 4);
	ptr = __freadptr(f, &size);
	assert(ptr != NULL);
	assert(size == sizeof(data) - 6);
	assert(*ptr == ',');
	long pos = ftell(f);
	assert(pos == 5);
	c = fgetc(f);
	assert(c == ',');

	pushed = ungetc('Y', f);
	assert(pushed == 'Y');
	__freadptrinc(f, 1);
	pos = ftell(f);
	assert(pos == 6);
	c = fgetc(f);
	assert(c == ' ');

	char rest[sizeof(data)];
	size_t nread = fread(rest, 1, sizeof(rest), f);
	assert(nread == sizeof(data) - 8);
	ptr = __freadptr(f, &size);
	assert(ptr == NULL);

	fseek(f, 0, SEEK_END);
	fputc('!', f);
	ptr = __freadptr(f, &size);
	assert(ptr == NULL);

	fclose(f);
#endif

	return 0;
}
