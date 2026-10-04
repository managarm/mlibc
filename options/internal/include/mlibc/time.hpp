#pragma once

#include <bits/size_t.h>
#include <frg/vector.hpp>
#include <mlibc/ctype.hpp>
#include <mlibc/locale.hpp>
#include <ranges>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace mlibc {

template <typename Char>
struct StrftimePolicy;

template <>
struct StrftimePolicy<char> {
	static constexpr const char *Percent = "%%";
	static constexpr const char *Newline = "\n";
	static constexpr const char *Tab = "\t";
	static constexpr const char *D = "%d";
	static constexpr const char *DotStarD = "%.*d";
	static constexpr const char *S = "%s";
	static constexpr const char *nativeS = "%s";
	static constexpr const char *TwoD = "%2d";
	static constexpr const char *Dot2D = "%.2d";
	static constexpr const char *Dot3D = "%.3d";
	static constexpr const char *Dot2I = "%.2i";
	static constexpr const char *DFormat = "%.2d/%.2d/%.2d";
	static constexpr const char *FFormat = "%d-%.2d-%.2d";
	static constexpr const char *RFormat = "%.2i:%.2i";
	static constexpr const char *TFormat = "%.2i:%.2i:%.2i";
	static constexpr const char *UFormat = "%02d";
	static constexpr const char *rFormat = "%.2i:%.2i:%.2i %s";
	static constexpr const char *zFormat = "%c%04d";

	static const char *tFmt(localeinfo *l) { return mlibc::nl_langinfo_l(T_FMT, l); }
	static const char *dFmt(localeinfo *l) { return mlibc::nl_langinfo_l(D_FMT, l); }
	static const char *dtFmt(localeinfo *l) { return mlibc::nl_langinfo_l(D_T_FMT, l); }
	static const char *amStr(localeinfo *l) { return mlibc::nl_langinfo_l(AM_STR, l); }
	static const char *pmStr(localeinfo *l) { return mlibc::nl_langinfo_l(PM_STR, l); }
};

template <>
struct StrftimePolicy<wchar_t> {
	static constexpr const wchar_t *Percent = L"%%";
	static constexpr const wchar_t *Newline = L"\n";
	static constexpr const wchar_t *Tab = L"\t";
	static constexpr const wchar_t *D = L"%d";
	static constexpr const wchar_t *DotStarD = L"%.*d";
	static constexpr const wchar_t *S = L"%s";
	static constexpr const wchar_t *nativeS = L"%ls";
	static constexpr const wchar_t *TwoD = L"%2d";
	static constexpr const wchar_t *Dot2D = L"%.2d";
	static constexpr const wchar_t *Dot3D = L"%.3d";
	static constexpr const wchar_t *Dot2I = L"%.2i";
	static constexpr const wchar_t *DFormat = L"%.2d/%.2d/%.2d";
	static constexpr const wchar_t *FFormat = L"%d-%.2d-%.2d";
	static constexpr const wchar_t *RFormat = L"%.2i:%.2i";
	static constexpr const wchar_t *TFormat = L"%.2i:%.2i:%.2i";
	static constexpr const wchar_t *UFormat = L"%02d";
	static constexpr const wchar_t *rFormat = L"%.2i:%.2i:%.2i %ls";
	static constexpr const wchar_t *zFormat = L"%c%04d";

	static const wchar_t *tFmt(localeinfo *l) {
		return l->time.get(_NL_WT_FMT).asWideString().data();
	}
	static const wchar_t *dFmt(localeinfo *l) {
		return l->time.get(_NL_WD_FMT).asWideString().data();
	}
	static const wchar_t *dtFmt(localeinfo *l) {
		return l->time.get(_NL_WD_T_FMT).asWideString().data();
	}
	static const wchar_t *amStr(localeinfo *l) {
		return l->time.get(_NL_WAM_STR).asWideString().data();
	}
	static const wchar_t *pmStr(localeinfo *l) {
		return l->time.get(_NL_WPM_STR).asWideString().data();
	}
};

template <typename Char>
size_t strftime(
    Char *__restrict dest,
    size_t max_size,
    const Char *__restrict format,
    const struct tm *__restrict tm,
    localeinfo *l
) {
	using P = StrftimePolicy<Char>;

	auto nprintf = [](Char *buf, size_t max_size, const Char *format, ...) {
		va_list args;
		va_start(args, format);
		int result = 0;

		if constexpr (std::is_same_v<Char, char>) {
			result = vsnprintf(buf, max_size, format, args);
		} else {
			result = vswprintf(buf, max_size, format, args);
		}

		va_end(args);
		return result;
	};

	auto pos_mod = [](std::integral auto a, std::integral auto m) noexcept {
		auto rem = a % m;
		return rem < 0 ? rem + m : rem;
	};

	auto is_leap = [&](int year) noexcept {
		int y = pos_mod(year, 400) + 300;
		return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
	};

	auto week_num = [&](const struct tm *tm) noexcept {
		// map wday to ISO: Monday = 0 ... Sunday = 6
		const int iso_wday = pos_mod(tm->tm_wday - 1, 7);

		// baseline Monday-aligned week count for current tm_yday
		int val = (tm->tm_yday - iso_wday + 7) / 7;

		// weekday of Jan 1 (0 = Sunday ... 6 = Saturday)
		const int jan1_wday = pos_mod(tm->tm_wday - tm->tm_yday, 7);

		// if Jan 1 is Tue (2), Wed (3), or Thu (4), week 1 started in/on Jan 1.
		if (pos_mod(jan1_wday - 2, 7) <= 2)
			val++;

		if (val == 0) {
			// underflow: Date belongs to the last week of the previous year.
			val = 52;

			const int dec31_prev_wday = pos_mod(jan1_wday - 1, 7);

			// safe subtraction: if tm_year == INT_MIN, pos_mod in is_leap avoids underflow
			const int prev_year =
			    (tm->tm_year == INT_MIN) ? (INT_MAX - (400 - 1)) : (tm->tm_year - 1);

			// previous year has 53 weeks if Dec 31 was Thu (4), or Fri (5) in a leap year.
			if (dec31_prev_wday == 4 || (dec31_prev_wday == 5 && is_leap(prev_year))) {
				val++;
			}
		} else if (val == 53) {
			// overflow: Year has 53 weeks only if Jan 1 was Thu (4), or Wed (3) in a leap year.
			const bool has_53_weeks = (jan1_wday == 4) || (jan1_wday == 3 && is_leap(tm->tm_year));
			if (!has_53_weeks) {
				val = 1;
			}
		}

		return val;
	};

	auto c = format;
	auto p = dest;

	while (*c) {
		int chunk;
		auto space = (dest + max_size) - p;
		__ensure(space >= 0);

		if (*c != '%') {
			if (!space)
				return 0;
			*p = *c;
			c++;
			p++;
			continue;
		}

		int minimum_width = 0;
		bool zero_pad = false;
		[[maybe_unused]] bool plus_flag = false;
		bool use_alternative_symbols = false;
		[[maybe_unused]] bool use_alternative_era_format = false;

		if (c[1] == '0') {
			zero_pad = true;
			c++;
		} else if (c[1] == '+') {
			// TODO: implement handling of this
			plus_flag = true;
			c++;
		}

		// read in width specifier
		if (c[1] >= '1' && c[1] <= '9') {
			minimum_width = 0;
			while (c[1] >= '0' && c[1] <= '9') {
				minimum_width = minimum_width * 10 + (c[1] - '0');
				c++;
			}
		}

		if (*(c + 1) == 'O') {
			constexpr auto valid = std::to_array<Char>(
			    {'B', 'b', 'd', 'e', 'H', 'I', 'm', 'M', 'S', 'u', 'U', 'V', 'w', 'W', 'y'}
			);
			if (std::ranges::contains(valid, c[2])) {
				use_alternative_symbols = true;
				c++;
			} else {
				*p = '%';
				p++;
				c++;
				*p = 'O';
				p++;
				c++;
				continue;
			}
		} else if (*(c + 1) == 'E') {
			constexpr auto valid = std::to_array<Char>({'c', 'C', 'x', 'X', 'y', 'Y'});
			if (std::ranges::contains(valid, c[2])) {
				use_alternative_era_format = true;
				c++;
			} else {
				*p = '%';
				p++;
				c++;
				*p = 'E';
				p++;
				c++;
				continue;
			}
		}

		auto print_digits = [&]<std::integral V, V Max>(V v, const Char *format) {
			if (use_alternative_symbols && v >= 0 && v <= Max) {
				auto altdigits = l->time.get(ALT_DIGITS).asString();
				auto t = altdigits | std::views::split('\0') | std::views::transform([](auto&& subrange) {
					return frg::string_view{
						std::ranges::data(subrange),
						static_cast<size_t>(std::ranges::distance(subrange))
					};
				});

				auto it = t.begin();
				if (std::ranges::advance(it, v, t.end()) == 0 && it != t.end()) {
					chunk = nprintf(p, space, P::S, (*it).data());
					return;
				}
			}

			chunk = nprintf(p, space, format, v);
		};

		switch (*++c) {
			case 'Y': {
				chunk = nprintf(p, space, P::D, 1900 + tm->tm_year);
				if (chunk >= space)
					return 0;

				if (zero_pad && minimum_width > chunk) {
					chunk = nprintf(p, space, P::DotStarD, minimum_width - chunk, 0);
					if (chunk >= space)
						return 0;
					p += chunk;
					space -= chunk;

					chunk = nprintf(p, space, P::D, 1900 + tm->tm_year);
					if (chunk >= space)
						return 0;
				}

				p += chunk;
				c++;
				break;
			}
			case 'm': {
				print_digits.template operator()<int, 12>(tm->tm_mon + 1, P::Dot2D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'd': {
				print_digits.template operator()<int, 31>(tm->tm_mday, P::Dot2D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'z': {
				auto min = tm->tm_gmtoff / 60;
				auto diff = ((min / 60) * 100) + (min % 60);
				chunk = nprintf(p, space, P::zFormat, diff >= 0 ? '+' : '-', abs(diff));
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'Z': {
				chunk = nprintf(p, space, P::S, tm->tm_zone);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'H': {
				print_digits.template operator()<int, 23>(tm->tm_hour, P::Dot2I);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'M': {
				print_digits.template operator()<int, 59>(tm->tm_min, P::Dot2I);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'S': {
				print_digits.template operator()<int, 60>(tm->tm_sec, P::Dot2D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'R': {
				chunk = nprintf(p, space, P::RFormat, tm->tm_hour, tm->tm_min);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'T': {
				chunk = nprintf(p, space, P::TFormat, tm->tm_hour, tm->tm_min, tm->tm_sec);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'F': {
				chunk =
				    nprintf(p, space, P::FFormat, 1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday);
				if (chunk >= space)
					return 0;

				// POSIX: if minimum width is less than 6, the behavior shall be as if it equalled 6.
				if (minimum_width)
					minimum_width = std::max(minimum_width, 6);

				if (zero_pad && minimum_width > chunk) {
					chunk = nprintf(p, space, P::DotStarD, minimum_width - chunk, 0);
					if (chunk >= space)
						return 0;
					p += chunk;
					space -= chunk;

					chunk = nprintf(
					    p, space, P::FFormat, 1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday
					);
					if (chunk >= space)
						return 0;
				}

				p += chunk;
				c++;
				break;
			}
			case 'D': {
				chunk = nprintf(
				    p, space, P::DFormat, tm->tm_mon + 1, tm->tm_mday, (tm->tm_year + 1900) % 100
				);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'a': {
				int day = tm->tm_wday;
				if (day < 0 || day > 6)
					__ensure(!"Day not in bounds.");

				auto str = [&] {
					if constexpr (std::is_same_v<Char, char>)
						return l->time.get(ABDAY_1 + day).asString();
					else
						return l->time.get(_NL_WABDAY_1 + day).asWideString();
				}();

				chunk = nprintf(p, space, P::nativeS, str.data());
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'b':
			case 'B':
			case 'h': {
				int mon = tm->tm_mon;
				if (mon < 0 || mon > 11)
					__ensure(!"Month not in bounds.");

				nl_item item = [&]() {
					if constexpr (std::is_same_v<Char, char>) {
						if (use_alternative_symbols) {
							return (*c == 'B') ? ALTMON_1 : ABALTMON_1;
						} else {
							return (*c == 'B') ? MON_1 : ABMON_1;
						}
					} else {
						if (use_alternative_symbols) {
							return (*c == 'B') ? _NL_WALTMON_1 : _NL_WABALTMON_1;
						} else {
							return (*c == 'B') ? _NL_WMON_1 : _NL_WABMON_1;
						}
					}
				}();

				if constexpr (std::is_same_v<Char, char>)
					chunk = nprintf(p, space, P::nativeS, l->time.get(item + mon).asString().data());
				else
					chunk = nprintf(p, space, P::nativeS, l->time.get(item + mon).asWideString().data());

				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'c': {
				return mlibc::strftime(dest, max_size, P::dtFmt(l), tm, l);
			}
			case 'e': {
				print_digits.template operator()<int, 31>(tm->tm_mday, P::TwoD);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'l': {
				int hour = tm->tm_hour;
				if (!hour)
					hour = 12;
				if (hour > 12)
					hour -= 12;
				chunk = nprintf(p, space, P::TwoD, hour);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'k': {
				chunk = nprintf(p, space, P::TwoD, tm->tm_hour);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'I': {
				int hour = tm->tm_hour;
				if (!hour)
					hour = 12;
				if (hour > 12)
					hour -= 12;

				print_digits.template operator()<int, 12>(hour, P::Dot2D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'p': {
				chunk = nprintf(p, space, P::S, (tm->tm_hour < 12) ? P::amStr(l) : P::pmStr(l));
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'P': {
				const Char *str = (tm->tm_hour < 12) ? P::amStr(l) : P::pmStr(l);

				frg::vector<Char, MemoryAllocator> str_lower{getAllocator()};
				str_lower.resize(frg::generic_strlen(str) + 1);

				for (size_t i = 0; str[i]; i++)
					str_lower[i] = mlibc::tolower_l(str[i], l);
				str_lower[frg::generic_strlen(str)] = '\0';

				chunk = nprintf(p, space, P::S, str_lower.data());
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'C': {
				if (zero_pad && minimum_width > 2) {
					chunk = nprintf(p, space, P::DotStarD, minimum_width - 2, 0);
					if (chunk >= space)
						return 0;
					p += chunk;
					space -= chunk;
				}

				chunk = nprintf(p, space, P::Dot2D, (1900 + tm->tm_year) / 100);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'y': {
				int year = (1900 + tm->tm_year) % 100;

				print_digits.template operator()<int, 99>(year, P::Dot2D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'j': {
				chunk = nprintf(p, space, P::Dot3D, tm->tm_yday + 1);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'A': {
				auto str = [&] {
					if constexpr (std::is_same_v<Char, char>)
						return l->time.get(DAY_1 + tm->tm_wday).asString();
					else
						return l->time.get(_NL_WDAY_1 + tm->tm_wday).asWideString();
				}();

				chunk = nprintf(p, space, P::nativeS, str.data());
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'r': {
				int hour = tm->tm_hour;
				if (!hour)
					hour = 12;
				if (hour > 12)
					hour -= 12;
				chunk = nprintf(
				    p,
				    space,
				    P::rFormat,
				    hour,
				    tm->tm_min,
				    tm->tm_sec,
				    (tm->tm_hour < 12) ? P::amStr(l) : P::pmStr(l)
				);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'w': {
				print_digits.template operator()<int, 6>(tm->tm_wday, P::D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case '%': {
				chunk = nprintf(p, space, P::Percent);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'n': {
				chunk = nprintf(p, space, P::Newline);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 't': {
				chunk = nprintf(p, space, P::Tab);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'x': {
				return mlibc::strftime(dest, max_size, P::dFmt(l), tm, l);
			}
			case 'X': {
				return mlibc::strftime(dest, max_size, P::tFmt(l), tm, l);
			}
			case 'u': {
				print_digits.template operator()<int, 7>(tm->tm_wday ? tm->tm_wday : 7, P::D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'U': {
				print_digits.template operator()<int, 53>((tm->tm_yday + 7 - tm->tm_wday) / 7, P::UFormat);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'W': {
				print_digits.template operator()<int, 53>((tm->tm_yday + 7 - (tm->tm_wday + 6) % 7) / 7, P::D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'V': {
				print_digits.template operator()<int, 53>(week_num(tm), P::D);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case '\0': {
				chunk = nprintf(p, space, P::Percent);
				if (chunk >= space)
					return 0;
				p += chunk;
				break;
			}
			default:
				mlibc::panicLogger() << "mlibc: strftime unknown format type: " << c << frg::endlog;
		}
	}

	auto space = (dest + max_size) - p;
	if (!space)
		return 0;

	*p = '\0';
	return (p - dest);
}

} // namespace mlibc
