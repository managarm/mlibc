#ifndef MLIBC_STRTOFP_HPP
#define MLIBC_STRTOFP_HPP

#include <bits/ensure.h>
#include <bits/nl_item.h>
#include <frg/string.hpp>
#include <mlibc/charcode.hpp>
#include <mlibc/charset.hpp>
#include <mlibc/ctype.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/strings.hpp>
#include <wchar.h>
#include <type_traits>

namespace mlibc {

template <typename Char>
struct StrToFpPolicy;

template <>
struct StrToFpPolicy<char> {
	static int string_compare(const char *l, const char *r) {
		return strcmp(l, r);
	}

	static int string_compare_n(const char *l, const char *r, size_t n) {
		return strncmp(l, r, n);
	}

	static int is_space(int c, mlibc::localeinfo *l) {
		return isspace_l(c, l);
	}

	static int is_digit(int c, mlibc::localeinfo *l) {
		return isdigit_l(c, l);
	}

	static constexpr const char *inf = "inf";
	static constexpr const char *infUpper = "INF";
	static constexpr const char *infinity = "infinity";
	static constexpr const char *infinityUpper = "INFINITY";
	static constexpr const char *nan = "nan";
	static constexpr const char *nanUpper = "NAN";
};

template <>
struct StrToFpPolicy<wchar_t> {
	static int string_compare(const wchar_t *l, const wchar_t *r) {
		return wcscmp(l, r);
	}

	static int string_compare_n(const wchar_t *l, const wchar_t *r, size_t n) {
		return wcsncmp(l, r, n);
	}

	static int is_space(int wc, mlibc::localeinfo *l) {
		auto cc = mlibc::platform_wide_charcode();
		mlibc::codepoint cp;
		if(auto e = cc->promote(wc, cp); e != mlibc::charcode_error::null)
			return 0;
		return mlibc::current_charset()->is_space(cp, static_cast<mlibc::localeinfo *>(l));
	}

	static int is_digit(int wc, mlibc::localeinfo *l) {
		auto cc = mlibc::platform_wide_charcode();
		mlibc::codepoint cp;
		if(auto e = cc->promote(wc, cp); e != mlibc::charcode_error::null)
			return 0;
		return mlibc::current_charset()->is_digit(cp, static_cast<mlibc::localeinfo *>(l));
	}

	static constexpr const wchar_t *inf = L"inf";
	static constexpr const wchar_t *infUpper = L"INF";
	static constexpr const wchar_t *infinity = L"infinity";
	static constexpr const wchar_t *infinityUpper = L"INFINITY";
	static constexpr const wchar_t *nan = L"nan";
	static constexpr const wchar_t *nanUpper = L"NAN";
};

template<typename T, typename Char>
T strtofp(const Char *str, Char **endptr, mlibc::localeinfo *l) {
	using Type = StrToFpPolicy<Char>;

	while(Type::is_space(*str, l))
		str++;

	if (Type::string_compare(str, Type::infUpper) == 0 || Type::string_compare(str, Type::inf) == 0) {
		if (endptr)
			*endptr = (Char *)str + 3;
		if constexpr (std::is_same_v<T, float>)
			return __builtin_inff();
		else if constexpr (std::is_same_v<T, double>)
			return __builtin_inf();
		else
			return __builtin_infl();
	} else if (Type::string_compare(str, Type::infinityUpper) == 0 || Type::string_compare(str, Type::infinity) == 0) {
		if (endptr)
			*endptr = (Char *)str + 8;
		if constexpr (std::is_same_v<T, float>)
			return __builtin_inff();
		else if constexpr (std::is_same_v<T, double>)
			return __builtin_inf();
		else
			return __builtin_infl();
	} else if (Type::string_compare_n(str, Type::nanUpper, 3) == 0 || Type::string_compare_n(str, Type::nan, 3) == 0) {
		if (endptr)
			*endptr = (Char *)str + 3;
		if constexpr (std::is_same_v<T, float>)
			return __builtin_nanf("");
		else if constexpr (std::is_same_v<T, double>)
			return __builtin_nan("");
		else
			return __builtin_nanl("");
	}

	wchar_t wideDecimalPoint[2] = { L'\0', L'\0' };

	auto decimal = [&]() -> frg::basic_string_view<Char> {
		if constexpr (std::is_same_v<Char, char>) {
			return l->numeric.get(__DECIMAL_POINT).asString();
		} else {
			wideDecimalPoint[0] = l->numeric.get(_NL_NUMERIC_DECIMAL_POINT_WC).asUint32();
			return wideDecimalPoint;
		}
	}();

	bool negative = *str == '-';
	if (*str == '+' || *str == '-')
		str++;

	bool hex = false;
	if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
		str += 2;
		hex = true;
	}

	T result = static_cast<T>(0);

	const Char *tmp = str;

	if (!hex) {
		while (true) {
			if (!Type::is_digit(*tmp, l))
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

	if (!Type::string_compare_n(tmp, decimal.data(), frg::generic_strnlen<Char>(decimal.data(), decimal.size()))) {
		tmp += frg::generic_strnlen<Char>(decimal.data(), decimal.size());

		if (!hex) {
			T d = static_cast<T>(10);

			while (true) {
				if (!Type::is_digit(*tmp, l))
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

			if (Type::is_digit(tmp[expOff], l)) {
				tmp += expOff;

				int exp = 0;
				while (true) {
					if (!Type::is_digit(*tmp, l))
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

			if (Type::is_digit(tmp[expOff], l)) {
				tmp += expOff;

				int exp = 0;
				while (true) {
					if (!Type::is_digit(*tmp, l))
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
		*endptr = const_cast<Char *>(tmp);
	if (negative)
		result = -result;

	return result;
}

} // namespace mlibc

#endif // MLIBC_STRTOFP_HPP
