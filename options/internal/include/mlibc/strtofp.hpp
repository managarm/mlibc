#ifndef MLIBC_STRTOFP_HPP
#define MLIBC_STRTOFP_HPP

#include <string.h>
#include <bits/ensure.h>

namespace mlibc {

template<typename T>
T strtofp(const char *str, char **endptr) {
	bool negative = *str == '-';
	if (*str == '+' || *str == '-')
		str++;

	if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
		__ensure(!"hex numbers in strtofp are unsupported");

	T result = static_cast<T>(0);

	const char *tmp = str;

	while (true) {
		if (!isdigit(*tmp))
			break;
		result *= static_cast<T>(10);
		result += static_cast<T>(*tmp - '0');
		tmp++;
	}

	if (*tmp == '.') {
		T d = static_cast<T>(10);

		if (*(tmp + 1) == '0' && (*(tmp + 2) == 'x' || *(tmp + 2) == 'X'))
			__ensure(!"hex numbers in strtofp are unsupported");
		tmp++;

		while (true) {
			if (!isdigit(*tmp))
				break;
			result += static_cast<T>(*tmp - '0') / d;
			d *= static_cast<T>(10);
			tmp++;
		}
	}

	if (endptr)
		*endptr = const_cast<char *>(tmp);
	if (negative)
		result = -result;

	return result;
}

}

#endif // MLIBC_STRTOFP_HPP
