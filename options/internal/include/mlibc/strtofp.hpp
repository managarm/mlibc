#ifndef MLIBC_STRTOFP_HPP
#define MLIBC_STRTOFP_HPP

#include <string.h>
#include <bits/ensure.h>
#include <type_traits>

namespace mlibc {

template<typename T>
T strtofp(const char *str, char **endptr) {
	if (strcmp(str, "INF") == 0 || strcmp(str, "inf") == 0) {
		if (endptr)
			*endptr = (char *)str + 3;
		if constexpr (std::is_same_v<T, float>)
			return __builtin_inff();
		else if constexpr (std::is_same_v<T, double>)
			return __builtin_inf();
		else
			return __builtin_infl();
	} else if (strcmp(str, "INFINITY") == 0 || strcmp(str, "infinity") == 0) {
	if (endptr)
		*endptr = (char *)str + 8;
	if constexpr (std::is_same_v<T, float>)
		return __builtin_inff();
	else if constexpr (std::is_same_v<T, double>)
		return __builtin_inf();
	else
		return __builtin_infl();
	} else if (strncmp(str, "NAN", 3) == 0 || strncmp(str, "nan", 3) == 0) {
		if (endptr)
			*endptr = (char *)str + 3;
		if constexpr (std::is_same_v<T, float>)
			return __builtin_nanf("");
		else if constexpr (std::is_same_v<T, double>)
			return __builtin_nan("");
		else
			return __builtin_nanl("");
	}

	bool negative = *str == '-';
	if (*str == '+' || *str == '-')
		str++;

	bool hex = false;
	if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
		str += 2;
		hex = true;
	}

	T result = static_cast<T>(0);

	const char *tmp = str;

	if (!hex) {
		while (true) {
			if (!isdigit(*tmp))
				break;
			result *= static_cast<T>(10);
			result += static_cast<T>(*tmp - '0');
			tmp++;
		}
	} else {
		while (true) {
			if (!isxdigit(*tmp))
				break;
			result *= static_cast<T>(16);
			result += static_cast<T>(*tmp <= '9' ? (*tmp - '0') : (tolower(*tmp) - 'a' + 10));
			tmp++;
		}
	}

	if (*tmp == '.') {
		tmp++;

		if (!hex) {
			T d = static_cast<T>(10);

			while (true) {
				if (!isdigit(*tmp))
					break;
				result += static_cast<T>(*tmp - '0') / d;
				d *= static_cast<T>(10);
				tmp++;
			}
		} else {
			T d = static_cast<T>(16);

			while (true) {
				if (!isxdigit(*tmp))
					break;
				result += static_cast<T>(*tmp <= '9' ? (*tmp - '0') : (tolower(*tmp) - 'a' + 10)) / d;
				d *= static_cast<T>(16);
				tmp++;
			}
		}
	}

	if (!hex) {
		if (*tmp == 'e' || *tmp == 'E') {
			tmp++;

			bool exp_negative = *tmp == '-';
			if (*tmp == '+' || *tmp == '-')
				tmp++;

			int exp = 0;
			while (true) {
				if (!isdigit(*tmp))
					break;
				exp *= 10;
				exp += *tmp - '0';
				tmp++;
			}

			if (!exp_negative) {
				for (int i = 0; i < exp; ++i) {
					result *= static_cast<T>(10);
				}
			} else {
				for (int i = 0; i < exp; ++i) {
					result /= static_cast<T>(10);
				}
			}
		}
	} else {
		if (*tmp == 'p' || *tmp == 'P') {
			tmp++;

			bool exp_negative = *tmp == '-';
			if (*tmp == '+' || *tmp == '-')
				tmp++;

			int exp = 0;
			while (true) {
				if (!isdigit(*tmp))
					break;
				exp *= 10;
				exp += *tmp - '0';
				tmp++;
			}

			if (!exp_negative) {
				for (int i = 0; i < exp; ++i) {
					result *= static_cast<T>(2);
				}
			} else {
				for (int i = 0; i < exp; ++i) {
					result /= static_cast<T>(2);
				}
			}
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
