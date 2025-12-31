#include <monetary.h>
#include <stdarg.h>

#include <frg/printf.hpp>
#include <mlibc/ctype.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/global-config.hpp>
#include <mlibc/locale.hpp>

namespace {

#define EMIT(...) if (!sink.append(__VA_ARGS__)) { errno = E2BIG; return -1; }

struct BufferSink {
	char *__restrict buffer;
	size_t maxsize;
	size_t usedsize = 0;

	bool append(char c) {
		if ((usedsize + 2) >= maxsize)
			return false;
		buffer[usedsize++] = c;
		return true;
	}

	bool append(const char *str) {
		return append(str, strlen(str));
	}

	bool append(const char *str, size_t len) {
		if ((usedsize + len + 1) >= maxsize)
			return false;
		memcpy(buffer + usedsize, str, len);
		usedsize += len;
		return true;
	}

	std::optional<ssize_t> finalize() {
		if (!append('\0'))
			return std::nullopt;

		__ensure(usedsize > 0);
		return usedsize - 1;
	}
};

ssize_t strfmon_internal(char *__restrict s, size_t maxsize, mlibc::localeinfo *locale, const char *__restrict format, va_list ap) {
	auto stripNullTerminator = [](frg::string_view v) {
		if (v.ends_with("") && v.size() > 1)
			return v.sub_string(0, v.size() - 1);
		return v;
	};

	auto pos_sign = stripNullTerminator(locale->monetary.get(POSITIVE_SIGN).asString());
	auto neg_sign = stripNullTerminator(locale->monetary.get(NEGATIVE_SIGN).asString());
	auto decimal_point = stripNullTerminator(locale->monetary.get(MON_DECIMAL_POINT).asString());
	if (!decimal_point.size())
		decimal_point = stripNullTerminator(locale->numeric.get(DECIMAL_POINT).asString());
	auto thousands_sep = stripNullTerminator(locale->monetary.get(MON_THOUSANDS_SEP).asString());
	if (!thousands_sep.size())
		thousands_sep = stripNullTerminator(locale->numeric.get(THOUSANDS_SEP).asString());
	auto grouping = locale->monetary.get(MON_GROUPING).asByteSpan();

	BufferSink sink{s, maxsize};

	while (*format) {
		if(*format != '%') {
			size_t plain_chars = 1;
			while(format[plain_chars] && format[plain_chars] != '%')
				plain_chars++;
			EMIT(format, plain_chars);
			format += plain_chars;
			continue;
		}

		format++;

		if (*format == '%') {
			EMIT('%');
			format++;
			continue;
		}

		bool group_thousands = true;
		bool suppress_currency_symbol = false;
		std::optional<size_t> field_width = std::nullopt;
		std::optional<size_t> left_precision = std::nullopt;
		std::optional<uint8_t> right_precision = std::nullopt;
		char padding = ' ';
		bool left_justify = false;
		enum class SignMode {
			normal,
			force_positive,
			parentheses,
		} sign_mode = SignMode::normal;

		bool int_format = false;

		// flags
		while(true) {
			if (*format == '-') {
				left_justify = true;
				++format;
				FRG_ASSERT(*format);
			} else if (*format == '+') {
				if (pos_sign == "" && neg_sign == "") {
					errno = EINVAL;
					return -1;
				}

				sign_mode = SignMode::force_positive;
				++format;
				FRG_ASSERT(*format);
			} else if (*format == '(') {
				sign_mode = SignMode::parentheses;
				++format;
				FRG_ASSERT(*format);
			} else if (*format == '^') {
				group_thousands = false;
				++format;
				FRG_ASSERT(*format);
			} else if (*format == '!') {
				suppress_currency_symbol = true;
				++format;
				FRG_ASSERT(*format);
			} else if (*format == '=') {
				++format;
				FRG_ASSERT(*format);
				padding = *format++;
				FRG_ASSERT(*format);
			} else {
				break;
			}
		}

		// field width
		if (mlibc::isdigit_l(*format, locale)) {
			size_t w = 0;
			while(*format >= '0' && *format <= '9') {
				w = w * 10 + (*format - '0');
				++format;
				FRG_ASSERT(*format);
			}
			field_width = w;
		}

		if (!field_width)
			left_justify = false;

		// left precision
		if (*format == '#') {
			format++;
			FRG_ASSERT(*format);
			size_t w = 0;
			while(*format >= '0' && *format <= '9') {
				w = w * 10 + (*format++ - '0');
				FRG_ASSERT(*format);
			}
			left_precision = w;
		}

		// right precision
		if (*format == '.') {
			format++;
			FRG_ASSERT(*format);
			size_t w = 0;
			while(*format >= '0' && *format <= '9') {
				w = w * 10 + (*format++ - '0');
				FRG_ASSERT(*format);
			}
			right_precision = w;
		}

		// conversion specifier
		if (*format == 'i') {
			int_format = true;
			format++;
		} else if (*format == 'n') {
			int_format = false;
			format++;
		} else {
			errno = EINVAL;
			return -1;
		}

		if (!right_precision)
			right_precision = locale->monetary.get(int_format ? INT_FRAC_DIGITS : FRAC_DIGITS).asUint32();

		if (right_precision.value() == 0xFF)
			right_precision = 2;

		double number = va_arg(ap, double);
		bool negative = __builtin_signbit(number);
		if (negative)
			number = -number;
		double integral_part;
		modf(number, &integral_part);
		auto integral_num = (uint64_t) integral_part;

		// determine appropriate formatting parameters given by the locale
		bool cs_precedes = [&]() -> bool {
			if (int_format) {
				auto v = locale->monetary.get(__builtin_signbit(number) ? INT_N_CS_PRECEDES : INT_P_CS_PRECEDES).asUint32();
				return v == 1;
			} else {
				auto v = locale->monetary.get(__builtin_signbit(number) ? N_CS_PRECEDES : P_CS_PRECEDES).asUint32();
				return v == 1;
			}
		}();

		uint8_t sep_by_space = [&]() -> uint8_t {
			if (int_format) {
				return locale->monetary.get(__builtin_signbit(number) ? INT_N_SEP_BY_SPACE : INT_P_SEP_BY_SPACE).asUint32();
			} else {
				return locale->monetary.get(__builtin_signbit(number) ? N_SEP_BY_SPACE : P_SEP_BY_SPACE).asUint32();
			}
		}();

		uint8_t sign_posn = [&]() -> uint8_t {
			if (int_format) {
				return locale->monetary.get(__builtin_signbit(number) ? INT_N_SIGN_POSN : INT_P_SIGN_POSN).asUint32();
			} else {
				return locale->monetary.get(__builtin_signbit(number) ? N_SIGN_POSN : P_SIGN_POSN).asUint32();
			}
		}();

		auto currency = stripNullTerminator(locale->monetary.get(int_format ? INT_CURR_SYMBOL : CURRENCY_SYMBOL).asString());
		frg::string_view currency_symbol{};

		if (!suppress_currency_symbol && currency.size()) {
			currency_symbol = currency;
			if (int_format)
				currency_symbol = currency.sub_string(0, 3);
		}

		frg::locale_options opts{
		    decimal_point.data(),
		    thousands_sep.data(),
		    grouping.size() ? reinterpret_cast<const char *>(grouping.data()) : ""
		};

		auto textLength = [](int i, unsigned base = 10, bool ignoreSign = false) {
			size_t length = (i < 0 && !ignoreSign) ? 1 : 0;
			do {
				i /= base;
				length++;
			} while (i != 0);
			return length;
		};

		auto sign_length = [&]() -> size_t {
			if (sign_mode == SignMode::parentheses || (sign_mode == SignMode::normal && sign_posn == 0))
				if (negative)
					return 2;
				else if (left_precision)
					return 1;
				else
					return 0;
			else if (!negative && sign_mode == SignMode::force_positive)
				return pos_sign.size();
			else if (negative)
				return neg_sign.size();
			else if (left_precision)
				return 1;
			return 0;
		}();

		// length of the currency string, including its separation to the numeric part
		size_t currency_length = suppress_currency_symbol ? 0 : ((int_format ? 3 : currency_symbol.size()) + sep_by_space);
		// length of the grouping characters in the integral part
		size_t extra_grouping = group_thousands ? frg::_fmt_basics::grouping_extra_characters(integral_num, 0, opts) : 0;
		// length of the integral part, excluding grouping, left-precision fill and sign
		size_t int_length = textLength(integral_num, 10, true);
		// length of the integral part, including grouping and sign, excluding left-precision fill
		size_t integral_length = sign_length + int_length + extra_grouping;
		// length of the fractional part, including the decimal point
		size_t fractional_length = (*right_precision > 0) ? (*right_precision + decimal_point.size()) : 0;
		size_t num_len = integral_length + fractional_length;

		size_t left_precision_fill_length = [&]() -> size_t {
			if (!left_precision)
				return 0;

			size_t extra_grouping_fill = 0;
			if (group_thousands) {
				// calculate the length of grouping characters that would appear for the largest
				// number that would not exceed the precision
				size_t maximum_grouping_len = frg::_fmt_basics::grouping_extra_characters(integral_num, *left_precision, opts);
				extra_grouping_fill = maximum_grouping_len - extra_grouping;
			}

			return (*left_precision > int_length) ? (*left_precision - int_length + extra_grouping_fill) : 0;
		}();

		// fill length due to field width
		size_t width_fill_length = (field_width && *field_width > (currency_length + num_len + left_precision_fill_length)) ? (*field_width - (currency_length + num_len + left_precision_fill_length)) : 0;

		if (mlibc::globalConfig().debugMonetaryLengths) {
			mlibc::infoLogger() << frg::fmt("Calculated lengths:") << frg::endlog;
			mlibc::infoLogger() << frg::fmt("\tcurrency={} sign_length={}", currency_length, sign_length) << frg::endlog;
			mlibc::infoLogger() << frg::fmt("\tint={} integral={} fractional={} num={}", int_length, integral_length, fractional_length, num_len) << frg::endlog;
			mlibc::infoLogger() << frg::fmt("\tleft_prec_fill={} width_fill={}", left_precision_fill_length, width_fill_length) << frg::endlog;
		}

		// perform actual formatting
		if (!left_justify) {
			for (size_t i = 0; i < width_fill_length; i++) {
				EMIT(padding);
			}
		}

		if (sign_mode == SignMode::parentheses || (sign_mode == SignMode::normal && sign_posn == 0)) {
			if (negative) {
				EMIT('(');
			} else if (left_precision) {
				EMIT(' ');
			}
		} else if (sign_posn == 1) {
			if (!negative && sign_mode == SignMode::force_positive) {
				EMIT(pos_sign.data());
			} else if (!negative && left_precision) {
				EMIT(' ');
			} else if (negative) {
				EMIT(neg_sign.data());
			}
		}

		if (!suppress_currency_symbol && currency.size() && cs_precedes) {
			if (sign_mode != SignMode::parentheses && sign_posn == 3) {
				if (!negative && sign_mode == SignMode::force_positive) {
					EMIT(pos_sign.data());
				} else if (!negative && left_precision) {
					EMIT(' ');
				} else if (negative) {
					EMIT(neg_sign.data());
				}
			}

			if (int_format) {
				EMIT(currency_symbol.data(), 3);
			} else {
				EMIT(currency_symbol.data());
			}

			if (sign_mode != SignMode::parentheses && sign_posn == 4) {
				if (!negative && sign_mode == SignMode::force_positive) {
					EMIT(pos_sign.data());
				} else if (!negative && left_precision) {
					EMIT(' ');
				} else if (negative) {
					EMIT(neg_sign.data());
				}
			}

			for(size_t i = 0; i < sep_by_space; i++) {
				EMIT(int_format ? currency[3] : ' ');
			}
		}

		if (left_precision && *left_precision > int_length) {
			for (size_t i = 0; i < left_precision_fill_length; i++) {
				EMIT(padding);
			}
		}

		frg::_fmt_basics::print_float(sink,
			number,
			0,
			*right_precision,
			padding,
			left_justify,
			false,
			false,
			group_thousands,
			false,
			false,
			false,
			opts
		);

		if (!suppress_currency_symbol && currency.size() && !cs_precedes) {
			for(size_t i = 0; i < sep_by_space; i++) {
				EMIT(int_format ? currency[3] : ' ');
			}

			if (sign_mode != SignMode::parentheses && sign_posn == 3) {
				if (!negative && sign_mode == SignMode::force_positive) {
					EMIT(pos_sign.data());
				} else if (!negative && left_precision) {
					EMIT(' ');
				} else if (negative) {
					EMIT(neg_sign.data());
				}
			}

			if (int_format) {
				EMIT(currency_symbol.data(), 3);
			} else {
				EMIT(currency_symbol.data());
			}

			if (sign_mode != SignMode::parentheses && sign_posn == 4) {
				if (!negative && sign_mode == SignMode::force_positive) {
					EMIT(pos_sign.data());
				} else if (!negative && left_precision) {
					EMIT(' ');
				} else if (negative) {
					EMIT(neg_sign.data());
				}
			}
		}

		if (sign_mode == SignMode::parentheses || (sign_mode == SignMode::normal && sign_posn == 0)) {
			if (negative) {
				EMIT(')');
			}
		} else if (sign_posn == 2) {
			if (!negative && sign_mode == SignMode::force_positive) {
				EMIT(pos_sign.data());
			} else if (!negative && left_precision) {
				EMIT(' ');
			} else if (negative) {
				EMIT(neg_sign.data());
			}
		}

		if (left_justify) {
			for(size_t i = 0; i < width_fill_length; i++) {
				EMIT(padding);
			}
		}
	}

	auto res = sink.finalize();
	if (!res) {
		errno = E2BIG;
		return -1;
	}

	return res.value();
}

} // namespace

ssize_t strfmon(char *__restrict s, size_t maxsize, const char *__restrict format, ...) {
	va_list va;
	va_start(va, format);
	ssize_t ret = strfmon_internal(s, maxsize, mlibc::getActiveLocale(), format, va);
	va_end(va);
	return ret;
}

ssize_t strfmon_l(char *__restrict s, size_t maxsize, locale_t locale, const char *__restrict format, ...) {
	va_list va;
	va_start(va, format);
	ssize_t ret = strfmon_internal(s, maxsize, static_cast<mlibc::localeinfo *>(locale), format, va);
	va_end(va);
	return ret;
}
