#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>

unsigned int error_message_count = 0;
int error_one_per_line = 0;
void (*error_print_progname)(void) = NULL;

void error(int status, int errnum, const char *format, ...) {
	va_list args;
	va_start(args, format);

	error_message_count++;
	
	fflush(stdout);
	if(error_print_progname) {
		error_print_progname();
	} else {
		fprintf(stderr, "%s: ", program_invocation_name);
	}
	vfprintf(stderr, format, args);
	va_end(args);

	if(errnum) {
		fprintf(stderr, ": %s\n", strerror(errnum));
	}

	if(status) {
		exit(status);
	}
}

void error_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format, ...) {
	va_list args;
	va_start(args, format);

	static bool first_call = true;
	static unsigned int last_line = 0;
	if(!(last_line == linenum && error_one_per_line && !first_call)) {
		first_call = false;
		last_line = linenum;
		error_message_count++;

		fflush(stdout);
		if(error_print_progname) {
			error_print_progname();
		} else {
			fprintf(stderr, "%s:", program_invocation_name);
		}
		fprintf(stderr, "%s:%u: ", filename, linenum);
		vfprintf(stderr, format, args);

		if(errnum) {
			fprintf(stderr, ": %s\n", strerror(errnum));
		}
	}
	va_end(args);

	if(status) {
		exit(status);
	}
}
