#ifndef MLIBC_ERRORS
#define MLIBC_ERRORS

#include <mlibc/debug.hpp>

#include <posix.frigg_bragi.hpp>
#include <fs.frigg_bragi.hpp>

constexpr bool logErrors = false;

int posixErrorToError(managarm::posix::Errors posixError);
int fsErrorToError(managarm::fs::Errors posixError);

#define POSIX_SUCCESS_OR_RETURN_ERROR(error) \
if(int __error = posixErrorToError(error) != 0) {  \
	if(logErrors) { \
		mlibc::infoLogger() << "mlibc: posix returned error " << (int)error << " (c error " << __error << ")\n" \
							   "  In file " __FILE__ " on line " HEL_STRINGIFY(__LINE__) "\n"; \
	} \
	return __error; \
}

#define FS_SUCCESS_OR_RETURN_ERROR(error) \
if(int __error = fsErrorToError(error) != 0) {  \
	if(logErrors) { \
		mlibc::infoLogger() << "mlibc: fs protocol returned error " << (int)error << " (c error " << __error << ")\n" \
							   "  In file " __FILE__ " on line " HEL_STRINGIFY(__LINE__) "\n"; \
	} \
	return __error; \
}

#endif
