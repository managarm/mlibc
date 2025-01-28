#include <err.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// va_list

void vwarn(const char *fmt, va_list params) {
	fprintf(stderr, "%s: ", program_invocation_short_name);
	if (fmt) {
		vfprintf(stderr, fmt, params);
                fwrite(": ", 1, 2, stderr);
	}
	perror(NULL);
}

void vwarnx(const char *fmt, va_list params) {
	fprintf(stderr, "%s: ", program_invocation_short_name);
	if (fmt) {
		vfprintf(stderr, fmt, params);
	}
	putc('\n', stderr);
}

__attribute__((__noreturn__)) void verr(int status, const char *fmt, va_list params) {
	vwarn(fmt, params);
	exit(status);
}

__attribute__((__noreturn__)) void verrx(int status, const char *fmt, va_list params) {
	vwarnx(fmt, params);
	exit(status);
}

// variadic

void warn(const char *fmt, ...) {
	va_list params;
	va_start(params, fmt);
	vwarn(fmt, params);
	va_end(params);
}

void warnx(const char *fmt, ...) {
	va_list params;
	va_start(params, fmt);
	vwarnx(fmt, params);
	va_end(params);
}

__attribute__((__noreturn__)) void err(int status, const char *fmt, ...) {
	va_list params;
	va_start(params, fmt);
	verr(status, fmt, params);
	va_end(params);
}

__attribute__((__noreturn__)) void errx(int status, const char *fmt, ...) {
	va_list params;
	va_start(params, fmt);
	verrx(status, fmt, params);
	va_end(params);
}
