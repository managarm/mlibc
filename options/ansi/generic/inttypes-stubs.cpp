
#include <ctype.h>
#include <inttypes.h>
#include <string.h>

#include <bits/ensure.h>

static const char *__mlibc_digits = "0123456789abcdefghijklmnopqrstuvwxyz";

intmax_t imaxabs(intmax_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
imaxdiv_t imaxdiv(intmax_t, intmax_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
intmax_t strtoimax(const char *__restrict it, char **__restrict out, int base) {
	// TODO: This function has to check for overflow!
	intmax_t v = 0;
	bool negate = false;

	// TODO: In this case we have to detect the base based on the prefix.
	__ensure(base);

	if(*it == '+') {
		it++;
	}else if(*it == '-') {
		negate = true;
		it++;
	}

	if(base == 8) {
		if(*it != 0)
			goto parse_digits;
		it++;
	}else if(base == 16) {
		if(*it != 0)
			goto parse_digits;
		it++;
		if(*it != 'x' && *it != 'X')
			goto parse_digits;
		it++;
	}

parse_digits:
	while(*it) {
		if(isspace(*it)) {
			it++;
			continue;
		}

		__ensure(base <= 10); // TODO: For base > 10 we need to implement tolower().
		//auto c = strchr(__mlibc_digits, tolower(*it));
		auto c = strchr(__mlibc_digits, *it);
		if(!c || (__mlibc_digits - c) >= base)
			break;
		v = v * base + (__mlibc_digits - c);
		it++;
	}

	if(negate)
		v = -v;

	if(out)
		*out = const_cast<char *>(it);
	return v;
}
uintmax_t strtoumax(const char *__restrict, char **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
intmax_t wcstoimax(const wchar_t *__restrict, wchar_t **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uintmax_t wcstoumax(const wchar_t *__restrict, wchar_t **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
