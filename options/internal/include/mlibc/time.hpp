#pragma once

#include <bits/size_t.h>
#include <frg/vector.hpp>
#include <mlibc/ctype.hpp>
#include <mlibc/locale.hpp>
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
	static constexpr const char *S = "%s";
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
	static constexpr const wchar_t *S = L"%s";
	static constexpr const wchar_t *TwoD = L"%2d";
	static constexpr const wchar_t *Dot2D = L"%.2d";
	static constexpr const wchar_t *Dot3D = L"%.3d";
	static constexpr const wchar_t *Dot2I = L"%.2i";
	static constexpr const wchar_t *DFormat = L"%.2d/%.2d/%.2d";
	static constexpr const wchar_t *FFormat = L"%d-%.2d-%.2d";
	static constexpr const wchar_t *RFormat = L"%.2i:%.2i";
	static constexpr const wchar_t *TFormat = L"%.2i:%.2i:%.2i";
	static constexpr const wchar_t *UFormat = L"%02d";
	static constexpr const wchar_t *rFormat = L"%.2i:%.2i:%.2i %s";
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

	auto c = format;
	auto p = dest;
	[[maybe_unused]] bool use_alternative_symbols = false;
	[[maybe_unused]] bool use_alternative_era_format = false;

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

		if (*(c + 1) == 'O') {
			std::array<Char, 15> valid{
			    {'B', 'b', 'd', 'e', 'H', 'I', 'm', 'M', 'S', 'u', 'U', 'V', 'w', 'W', 'y'}
			};
			auto next = *(c + 2);
			if (std::find(valid.begin(), valid.end(), next) != valid.end()) {
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
			std::array<Char, 6> valid{{'c', 'C', 'x', 'X', 'y', 'Y'}};
			auto next = *(c + 2);
			if (std::find(valid.begin(), valid.end(), next) != valid.end()) {
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

		switch (*++c) {
			case 'Y': {
				chunk = nprintf(p, space, P::D, 1900 + tm->tm_year);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'm': {
				chunk = nprintf(p, space, P::Dot2D, tm->tm_mon + 1);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'd': {
				chunk = nprintf(p, space, P::Dot2D, tm->tm_mday);
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
				chunk = nprintf(p, space, P::Dot2I, tm->tm_hour);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'M': {
				chunk = nprintf(p, space, P::Dot2I, tm->tm_min);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'S': {
				chunk = nprintf(p, space, P::Dot2D, tm->tm_sec);
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

				chunk = nprintf(p, space, P::S, mlibc::nl_langinfo_l(ABDAY_1 + day, l));
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

				nl_item item = (*c == 'B') ? MON_1 : ABMON_1;

				chunk = nprintf(p, space, P::S, mlibc::nl_langinfo_l(item + mon, l));
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
				chunk = nprintf(p, space, P::TwoD, tm->tm_mday);
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
				chunk = nprintf(p, space, P::Dot2D, hour);
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
				chunk = nprintf(p, space, P::Dot2D, (1900 + tm->tm_year) / 100);
				if (chunk >= space)
					return 0;
				p += chunk;
				c++;
				break;
			}
			case 'y': {
				chunk = nprintf(p, space, P::Dot2D, (1900 + tm->tm_year) % 100);
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
				chunk = nprintf(p, space, P::S, mlibc::nl_langinfo_l(DAY_1 + tm->tm_wday, l));
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
			case 'U': {
				chunk = nprintf(p, space, P::UFormat, (tm->tm_yday + 7 - tm->tm_wday) / 7);
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
