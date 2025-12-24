#ifndef MLIBC_STRTOFP_HPP
#define MLIBC_STRTOFP_HPP

#include <bits/ensure.h>
#include <mlibc/ctype.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/strings.hpp>
#include <type_traits>

namespace mlibc {

template<typename T>
T strtofp(const char *str, char **endptr, mlibc::localeinfo *l) {
	while(isspace_l(*str, l))
		str++;

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

	auto decimal = l->numeric.get(DECIMAL_POINT).asString();

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
			if (!isdigit_l(*tmp, l))
				break;
			result *= static_cast<T>(10);
			result += static_cast<T>(*tmp - '0');
			tmp++;
		}
	} else {
		while (true) {
			if (!isxdigit_l(*tmp, l))
				break;
			result *= static_cast<T>(16);
			result += static_cast<T>(*tmp <= '9' ? (*tmp - '0') : (tolower_l(*tmp, l) - 'a' + 10));
			tmp++;
		}
	}

	if (!strncmp(tmp, decimal.data(), strnlen(decimal.data(), decimal.size()))) {
		tmp += strnlen(decimal.data(), decimal.size());

		if (!hex) {
			T d = static_cast<T>(10);

			while (true) {
				if (!isdigit_l(*tmp, l))
					break;
				result += static_cast<T>(*tmp - '0') / d;
				d *= static_cast<T>(10);
				tmp++;
			}
		} else {
			T d = static_cast<T>(16);

			while (true) {
				if (!isxdigit_l(*tmp, l))
					break;
				result += static_cast<T>(*tmp <= '9' ? (*tmp - '0') : (tolower_l(*tmp, l) - 'a' + 10)) / d;
				d *= static_cast<T>(16);
				tmp++;
			}
		}
	}

	if (!hex) {
		if (*tmp == 'e' || *tmp == 'E') {
			// offset so we look ahead instead of incrementing tmp for a possibly-invalid exponent
			size_t expOff = 1;

			bool exp_negative = tmp[expOff] == '-';
			if (tmp[expOff] == '+' || tmp[expOff] == '-')
				expOff++;

			if (isdigit_l(tmp[expOff], l)) {
				tmp += expOff;

				int exp = 0;
				while (true) {
					if (!isdigit_l(*tmp, l))
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
		}
	} else {
		if (*tmp == 'p' || *tmp == 'P') {
			// offset so we look ahead instead of incrementing tmp for a possibly-invalid exponent
			size_t expOff = 1;

			bool exp_negative = tmp[expOff] == '-';
			if (tmp[expOff] == '+' || tmp[expOff] == '-')
				expOff++;

			if (isdigit_l(tmp[expOff], l)) {
				tmp += expOff;

				int exp = 0;
				while (true) {
					if (!isdigit_l(*tmp, l))
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
	}

	if (endptr)
		*endptr = const_cast<char *>(tmp);
	if (negative)
		result = -result;

	return result;
}

} // namespace mlibc

#endif // MLIBC_STRTOFP_HPP
