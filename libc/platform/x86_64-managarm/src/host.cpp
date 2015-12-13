
#include <string.h>
#include <unistd.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>

#pragma GCC visibility pop

int gethostname(char *buffer, size_t max_length) {
	const char *name = "cradle";
	frigg::infoLogger.log() << "mlibc: Broken time() called!" << frigg::EndLog();
	strncpy(buffer, name, max_length);
	return 0;
}

