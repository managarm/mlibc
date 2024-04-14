
#include <ctype.h>
#include <inttypes.h>
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

static const char *__mlibc_digits = "0123456789abcdefghijklmnopqrstuvwxyz";

intmax_t imaxabs(intmax_t num) {
	return num < 0 ? -num : num;
}
imaxdiv_t imaxdiv(intmax_t number, intmax_t denom) {
	imaxdiv_t r;
	r.quot = number / denom;
	r.rem = number % denom;
	return r;
}

template <class T> T strtoxmax(const char *it, char **out, int base) {
	T v = 0;
	bool negate = false;
	const unsigned char *s = (const unsigned char *)it;
	int c;

	if(std::is_signed<T>::value) {
		if(*s == '+') {
			s++;
		}else if(*s == '-') {
			negate = true;
			s++;
		}
	}

	do {
		c = *s++;
	} while (isspace(c));
	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

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
		if(!c || (c - __mlibc_digits) >= base)
			break;
		v = v * base + (c - __mlibc_digits);
		it++;
	}

	if(std::is_signed<T>::value) {
		if(negate)
			v = -v;
	}

	if(out)
		*out = const_cast<char *>(it);
	return v;
}

intmax_t strtoimax(const char *it, char **out, int base) {
	// TODO: This function has to check for overflow!
	return strtoxmax<intmax_t>(it, out, base);
}
uintmax_t strtoumax(const char *it, char **out, int base) {
	return strtoxmax<uintmax_t>(it, out, base);
}
intmax_t wcstoimax(const wchar_t *__restrict, wchar_t **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uintmax_t wcstoumax(const wchar_t *__restrict, wchar_t **__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
