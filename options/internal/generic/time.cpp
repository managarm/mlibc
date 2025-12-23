#include <mlibc/ctype.hpp>
#include <mlibc/time.hpp>
#include <stdio.h>
#include <stdlib.h>

namespace mlibc {

size_t strftime(
    char *__restrict dest,
    size_t max_size,
    const char *__restrict format,
    const struct tm *__restrict tm,
    localeinfo *l
) {
	auto c = format;
	auto p = dest;
	[[maybe_unused]] bool use_alternative_symbols = false;
	[[maybe_unused]] bool use_alternative_era_format = false;

	while(*c) {
		int chunk;
		auto space = (dest + max_size) - p;
		__ensure(space >= 0);

		if(*c != '%') {
			if(!space)
				return 0;
			*p = *c;
			c++;
			p++;
			continue;
		}

		if(*(c + 1) == 'O') {
			std::array<char, 15> valid{{'B', 'b', 'd', 'e', 'H', 'I', 'm', 'M', 'S', 'u', 'U', 'V', 'w', 'W', 'y'}};
			auto next = *(c + 2);
			if(std::find(valid.begin(), valid.end(), next) != valid.end()) {
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
		} else if(*(c + 1) == 'E') {
			std::array<char, 6> valid{{'c', 'C', 'x', 'X', 'y', 'Y'}};
			auto next = *(c + 2);
			if(std::find(valid.begin(), valid.end(), next) != valid.end()) {
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

		switch(*++c) {
		case 'Y': {
			chunk = snprintf(p, space, "%d", 1900 + tm->tm_year);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'm': {
			chunk = snprintf(p, space, "%.2d", tm->tm_mon + 1);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'd': {
			chunk = snprintf(p, space, "%.2d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'z': {
			auto min = tm->tm_gmtoff / 60;
			auto diff = ((min / 60) * 100) + (min % 60);
			chunk = snprintf(p, space, "%c%04d", diff >= 0 ? '+' : '-', abs(diff));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'Z': {
			chunk = snprintf(p, space, "%s", tm->tm_zone);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'H': {
			chunk = snprintf(p, space, "%.2i", tm->tm_hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'M': {
			chunk = snprintf(p, space, "%.2i", tm->tm_min);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'S': {
			chunk = snprintf(p, space, "%.2d", tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'R': {
			chunk = snprintf(p, space, "%.2i:%.2i", tm->tm_hour, tm->tm_min);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'T': {
			chunk = snprintf(p, space, "%.2i:%.2i:%.2i", tm->tm_hour, tm->tm_min, tm->tm_sec);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'F': {
			chunk = snprintf(p, space, "%d-%.2d-%.2d", 1900 + tm->tm_year, tm->tm_mon + 1,
					tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'D': {
			chunk = snprintf(p, space, "%.2d/%.2d/%.2d", tm->tm_mon + 1, tm->tm_mday, (tm->tm_year + 1900) % 100);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'a': {
			int day = tm->tm_wday;
			if(day < 0 || day > 6)
				__ensure(!"Day not in bounds.");

			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo_l(ABDAY_1 + day, l));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'b':
		case 'B':
		case 'h': {
			int mon = tm->tm_mon;
			if(mon < 0 || mon > 11)
				__ensure(!"Month not in bounds.");

			nl_item item = (*c == 'B') ? MON_1 : ABMON_1;

			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo_l(item + mon, l));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'c': {
			// int day = tm->tm_wday;
			// if(day < 0 || day > 6)
			// 	__ensure(!"Day not in bounds.");

			// int mon = tm->tm_mon;
			// if(mon < 0 || mon > 11)
			// 	__ensure(!"Month not in bounds.");

			// chunk = snprintf(p, space, "%s %s %2d %.2i:%.2i:%.2d %d", mlibc::nl_langinfo_l(ABDAY_1 + day, l),
			// 		mlibc::nl_langinfo_l(ABMON_1 + mon, l), tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, 1900 + tm->tm_year);
			// if(chunk >= space)
			// 	return 0;
			// p += chunk;
			// c++;
			// break;
			return mlibc::strftime(dest, max_size, mlibc::nl_langinfo_l(D_T_FMT, l), tm, l);
		}
		case 'e': {
			chunk = snprintf(p, space, "%2d", tm->tm_mday);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'l': {
			int hour = tm->tm_hour;
			if(!hour)
				hour = 12;
			if(hour > 12)
				hour -= 12;
			chunk = snprintf(p, space, "%2d", hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'k': {
			chunk = snprintf(p, space, "%2d", tm->tm_hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'I': {
			int hour = tm->tm_hour;
			if(!hour)
				hour = 12;
			if(hour > 12)
				hour -= 12;
			chunk = snprintf(p, space, "%.2d", hour);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'p': {
			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo_l((tm->tm_hour < 12) ? AM_STR : PM_STR, l));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'P': {
			char *str = mlibc::nl_langinfo_l((tm->tm_hour < 12) ? AM_STR : PM_STR, l);
			char *str_lower = reinterpret_cast<char *>(getAllocator().allocate(strlen(str) + 1));
			for(size_t i = 0; str[i]; i++)
				str_lower[i] = mlibc::tolower_l(str[i], l);
			str_lower[strlen(str)] = '\0';

			chunk = snprintf(p, space, "%s", str_lower);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'C': {
			chunk = snprintf(p, space, "%.2d", (1900 + tm->tm_year) / 100);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'y': {
			chunk = snprintf(p, space, "%.2d", (1900 + tm->tm_year) % 100);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'j': {
			chunk = snprintf(p, space, "%.3d", tm->tm_yday + 1);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'A': {
			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo_l(DAY_1 + tm->tm_wday, l));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'r': {
			int hour = tm->tm_hour;
			if(!hour)
				hour = 12;
			if(hour > 12)
				hour -= 12;
			chunk = snprintf(p, space, "%.2i:%.2i:%.2i %s", hour, tm->tm_min, tm->tm_sec,
				mlibc::nl_langinfo_l((tm->tm_hour < 12) ? AM_STR : PM_STR, l));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case '%': {
			chunk = snprintf(p, space, "%%");
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'n': {
			chunk = snprintf(p, space, "\n");
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 't': {
			chunk = snprintf(p, space, "\t");
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'x': {
			return mlibc::strftime(dest, max_size, mlibc::nl_langinfo_l(D_FMT, l), tm, l);
		}
		case 'X': {
			return mlibc::strftime(dest, max_size, mlibc::nl_langinfo_l(T_FMT, l), tm, l);
		}
		case 'U': {
			chunk = snprintf(p, space, "%02d", (tm->tm_yday + 7 - tm->tm_wday) / 7);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case '\0': {
			chunk = snprintf(p, space, "%%");
			if(chunk >= space)
				return 0;
			p += chunk;
			break;
		}
		default:
			mlibc::panicLogger() << "mlibc: strftime unknown format type: " << c << frg::endlog;
		}
	}

	auto space = (dest + max_size) - p;
	if(!space)
		return 0;

	*p = '\0';
	return (p - dest);
}

} // namespace mlibc
