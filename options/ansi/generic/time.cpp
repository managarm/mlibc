#include <algorithm>
#include <array>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <wchar.h>
#include <stdlib.h>
#include <ctype.h>

#if __MLIBC_POSIX_OPTION
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/file-window.hpp>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/lock.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/bitutil.hpp>
#include <mlibc/strings.hpp>

#include <frg/mutex.hpp>

// The DST rules to use if TZ has no rules and we can't load posixinfo.
// POSIX does not specify the default DST rules, for historical reasons
// America/New_York is a common default.
#define TZ_DEFAULT_RULE_STRING ",M3.2.0,M11.1.0"

const char __utc[] = "UTC";

// Variables defined by POSIX.
int daylight;
long timezone;
char *tzname[2];

static FutexLock __time_lock;

// Function taken from musl
clock_t clock(void) {
	struct timespec ts;

	if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts))
		return -1;

	if(ts.tv_sec > LONG_MAX / 1000000 || ts.tv_nsec / 1000 > LONG_MAX - 1000000 * ts.tv_sec)
		return -1;

	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

double difftime(time_t a, time_t b) {
	return a - b;
}

time_t mktime(struct tm *tm) {
	return timegm(tm);
}

/* There is no other implemented value than TIME_UTC; all other values
 * are considered erroneous. */
// Function taken from musl
int timespec_get(struct timespec *ts, int base) {
	if(base != TIME_UTC)
		return 0;
	int ret = clock_gettime(CLOCK_REALTIME, ts);
	return ret < 0 ? 0 : base;
}

char *asctime(const struct tm *ptr) {
	static char buf[26];
	return asctime_r(ptr, buf);
}

char *ctime(const time_t *timer) {
	struct tm *tm = localtime(timer);
	if(!tm) {
		return nullptr;
	}
	return asctime(tm);
}

struct tm *gmtime(const time_t *unix_gmt) {
	static thread_local struct tm per_thread_tm;
	return gmtime_r(unix_gmt, &per_thread_tm);
}

struct tm *localtime(const time_t *unix_gmt) {
	tzset();
	static thread_local struct tm per_thread_tm;
	return localtime_r(unix_gmt, &per_thread_tm);
}

size_t strftime(char *__restrict dest, size_t max_size,
		const char *__restrict format, const struct tm *__restrict tm) {
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
			chunk = snprintf(p, space, "%s", "UTC");
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

			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo(ABDAY_1 + day));
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

			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo(item + mon));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'c': {
			int day = tm->tm_wday;
			if(day < 0 || day > 6)
				__ensure(!"Day not in bounds.");

			int mon = tm->tm_mon;
			if(mon < 0 || mon > 11)
				__ensure(!"Month not in bounds.");

			chunk = snprintf(p, space, "%s %s %2d %.2i:%.2i:%.2d %d", mlibc::nl_langinfo(ABDAY_1 + day),
					mlibc::nl_langinfo(ABMON_1 + mon), tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, 1900 + tm->tm_year);
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
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
			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo((tm->tm_hour < 12) ? AM_STR : PM_STR));
			if(chunk >= space)
				return 0;
			p += chunk;
			c++;
			break;
		}
		case 'P': {
			char *str = mlibc::nl_langinfo((tm->tm_hour < 12) ? AM_STR : PM_STR);
			char *str_lower = reinterpret_cast<char *>(getAllocator().allocate(strlen(str) + 1));
			for(size_t i = 0; str[i]; i++)
				str_lower[i] = tolower(str[i]);
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
			chunk = snprintf(p, space, "%s", mlibc::nl_langinfo(DAY_1 + tm->tm_wday));
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
				mlibc::nl_langinfo((tm->tm_hour < 12) ? AM_STR : PM_STR));
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
			return strftime(dest, max_size, mlibc::nl_langinfo(D_FMT), tm);
		}
		case 'X': {
			return strftime(dest, max_size, mlibc::nl_langinfo(T_FMT), tm);
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

size_t wcsftime(wchar_t *__restrict, size_t, const wchar_t *__restrict,
		const struct tm *__restrict) {
	mlibc::infoLogger() << "mlibc: wcsftime is a stub" << frg::endlog;
	return 0;
}

namespace {

// Given a pointer to a timezone string, extract a number and check if it's in
// range; if it's not, return NULL. Otherwise, return a pointer to the first
// character not part of the number.
template<typename T>
const char *getnum(const char *str, T *nump, T min, T max) {
	if (str == nullptr || !isdigit(*str))
		return nullptr;

	char c = *str;
	T num = 0;
	do {
		num = num * 10 + (c - '0');
		if (num > max)
			return nullptr;
		c = *++str;
	} while (isdigit(c));
	if (num < min)
		return nullptr;
	*nump = num;
	return str;
}

// Given a pointer into a timezone string, extract an offset, in
// [+-]hh[:mm[:ss]] form. If any error occurs, return NULL. Otherwise, return a
// pointer to the first character not part of the time.
const char *getoffset(const char *str, long *offset) {
	bool negative = false;
	if (*str == '-') {
		negative = true;
		str++;
	} else if (*str == '+') {
		str++;
	}

	unsigned int num;
	// `24 * 7 - 1` allows for quasi-POSIX rules like "M10.4.6/26", which does
	// not conform to POSIX, but specifies the equivalent of "02:00 on the
	// first Sunday on or after 23 Oct".
	str = getnum<unsigned int>(str, &num, 0, 24 * 7 - 1);
	if (str == nullptr)
		return nullptr;
	*offset = num * 60 * 60;
	if (*str == ':') {
		str++;
		str = getnum<unsigned int>(str, &num, 0, 59);
		if (str == nullptr)
			return nullptr;
		*offset += num * 60;
		if (*str == ':') {
			str++;
			// Allows for leap seconds.
			str = getnum<unsigned int>(str, &num, 0, 60);
			if (str == nullptr)
				return nullptr;
			*offset += num;
		}
	}

	if (negative)
		*offset *= -1;

	return str;
}

enum RuleType {
	TZFILE, // mlibc-internal rule type for TZ files
	JULIAN_DAY, // Jn = Julian day
	DAY_OF_YEAR, // n = day of year
	MONTH_NTH_DAY_OF_WEEK, // Mm.n.d = month, week, day of week
};

struct Rule {
	RuleType type;
	uint16_t day;
	uint8_t week;
	uint8_t month;
	long time;
};

// Given a pointer into a timezone string, extract a rule in the form
// date[/time]. If a valid rule is not found, return NULL; otherwise, return a
// pointer to the first character not part of the rule.
const char *getrule(const char *str, Rule *rule) {
	if (*str == 'J') { // Julian day
		rule->type = JULIAN_DAY;
		str++;
		str = getnum<uint16_t>(str, &rule->day, 1, 365);
	} else if (*str == 'M') { // Month, week, day
		rule->type = MONTH_NTH_DAY_OF_WEEK;
		str++;
		str = getnum<uint8_t>(str, &rule->month, 1, 12);
		if (str == nullptr)
			return nullptr;
		if (*str++ != '.')
			return nullptr;
		str = getnum<uint8_t>(str, &rule->week, 1, 5);
		if (str == nullptr)
			return nullptr;
		if (*str++ != '.')
			return nullptr;
		str = getnum<uint16_t>(str, &rule->day, 0, 6);
	} else if (isdigit(*str)) { // Day of year
		rule->type = DAY_OF_YEAR;
		str = getnum<uint16_t>(str, &rule->day, 0, 365);
	} else {
		return nullptr;
	}

	if (str == nullptr)
		return nullptr;

	if (*str == '/') {
		str++;
		str = getoffset(str, &rule->time);
	} else {
		// Fallback to 02:00:00.
		rule->time = 2 * 60 * 60;
	}

	return str;
}

struct[[gnu::packed]] ttinfo {
	int32_t tt_gmtoff;
	unsigned char tt_isdst;
	unsigned char tt_abbrind;
};

// Let's just assume there's a maximum of two for now.
ttinfo tt_infos[2];
Rule rules[2];

bool parse_tz(const char *tz, char *tz_name, char *tz_name_dst, size_t tz_name_max) {
	// POSIX defines :*characters* as a valid but implementation-defined format.
	// glibc ignores the initial colon and parses the rest as TZ.
	if (*tz == ':')
		tz++;

	// The timezone name may be wrapped in angle brackets, in which case we
	// parse them in quoted mode.
	bool quoted = false;
	if (*tz == '<') {
		quoted = true;
		tz++;
	}

	// Try parsing the timezone name.
	auto *tzn = tz;
	size_t tzn_len = 0;
	for (;; tz++) {
		tzn_len = tz - tzn;
		if (*tz == '\0')
			break;

		if (tzn_len > tz_name_max)
			return true;

		// Advance until the end of the timezone name.
		if (isalpha(*tz))
			continue;
		if (quoted && (*tz == '+' || *tz == '-' || isdigit(*tz)))
			continue;

		// Check if the timezone name has a valid length.
		if (tzn_len < 3)
			return true;

		// Consume the terminating angle bracket.
		if (quoted && *tz == '>') {
			tz++;
		} else if (quoted) {
			mlibc::infoLogger() << "mlibc: TZ name has unclosed angle bracket" << frg::endlog;
			return true;
		}

		break;
	}

	long offset = 0;
	tz = getoffset(tz, &offset);
	if (tz == nullptr)
		return true;

	// If we're here, this MUST be of the POSIX timezone format.
	// Write the TZ name to the buffer passed to the function.
	memcpy(tz_name, tzn, tzn_len);
	tz_name[tzn_len] = '\0';

	timezone = offset;

	tt_infos[0].tt_gmtoff = -offset;
	tt_infos[0].tt_isdst = false;
	tt_infos[0].tt_abbrind = 0;

	// If there's nothing left to parse, we should set tz_name_dst to tz_name.
	// This matches glibc behaviour.
	if (*tz == '\0') {
		memcpy(tz_name_dst, tzn, tzn_len);
		tz_name_dst[tzn_len] = '\0';
		return false;
	}

	// From now on, we won't return an error but silently stop parsing. This
	// makes a parsing error on the rest of the TZ environment variable not
	// prevent setting the values we parsed before this point. This matches
	// glibc behaviour.

	// The timezone name may be wrapped in angle brackets, in which case we
	// parse them in quoted mode.
	quoted = false;
	if (*tz == '<') {
		quoted = true;
		tz++;
	}

	// Try parsing the alternate timezone (DST) name.
	auto *tzn_dst = tz;
	size_t tzn_len_dst = 0;
	for (;; tz++) {
		tzn_len_dst = tz - tzn_dst;
		if (*tz == '\0')
			break;

		if (tzn_len_dst > tz_name_max)
			return false;

		// Advance until the end of the timezone name.
		if (isalpha(*tz))
			continue;
		if (quoted && (*tz == '+' || *tz == '-' || isdigit(*tz)))
			continue;

		// Check if the timezone name has a valid length.
		if (tzn_len_dst < 3)
			return false;

		// Consume the terminating angle bracket.
		if (quoted && *tz == '>') {
			tz++;
		} else if (quoted) {
			mlibc::infoLogger() << "mlibc: TZ name has unclosed angle bracket" << frg::endlog;
			return false;
		}

		break;
	}

	// Write the TZ name to the buffer passed to the function.
	memcpy(tz_name_dst, tzn_dst, tzn_len_dst);
	tz_name_dst[tzn_len_dst] = '\0';

	// Fallback to 1 hour ahead of standard time.
	long offset_dst = offset - 60 * 60;
	if (*tz != '\0' && *tz != ',') {
		tz = getoffset(tz, &offset_dst);
		if (tz == nullptr)
			return false;
	}

	// TODO: Attempt to fallback to posixrules before falling back to this.
	if (*tz == '\0')
		tz = TZ_DEFAULT_RULE_STRING;

	if (*tz == ',') {
		tz++;
		tz = getrule(tz, &rules[0]);
		if (tz == nullptr)
			return false;
		if (*tz != ',')
			return false;
		tz++;
		tz = getrule(tz, &rules[1]);
		if (tz == nullptr)
			return false;
		if (*tz != '\0')
			return false;
	} else {
		return false;
	}

	tt_infos[1].tt_gmtoff = -offset_dst;
	tt_infos[1].tt_isdst = true;
	tt_infos[1].tt_abbrind = 0;

	daylight = 1;

	return false;
}

struct tzfile {
	uint8_t magic[4];
	uint8_t version;
	uint8_t reserved[15];
	uint32_t tzh_ttisgmtcnt;
	uint32_t tzh_ttisstdcnt;
	uint32_t tzh_leapcnt;
	uint32_t tzh_timecnt;
	uint32_t tzh_typecnt;
	uint32_t tzh_charcnt;
};

frg::string<MemoryAllocator> parse_tzfile_path(const char *tz) {
	// POSIX defines :*characters* as a valid but implementation-defined format.
	// This was originally introduced as a way to support geographical
	// timezones in the format :Area/Location, but the colon was dropped in POSIX.
	if (*tz == ':')
		tz++;

	frg::string<MemoryAllocator> path {getAllocator()};
	// TODO: generic path helpers in options/internal?
	if (*tz == '/') {
		path += tz;
	} else if (*tz == '.') {
		// FIXME: Figure out what we actually need to do in this case, consider
		//        supporting relative paths or defaulting to UTC instead.
		mlibc::infoLogger() << "mlibc: relative path in TZ not supported, "
			"defaulting to /etc/localtime" << frg::endlog;
		path += "/etc/localtime";
	} else {
		const char *tzdir = getenv("TZDIR");
		if (tzdir == nullptr || *tzdir == '\0') {
			tzdir = "/usr/share/zoneinfo";
		} else if (*tzdir != '/') {
			mlibc::infoLogger() << "mlibc: non-absolute path in TZDIR not "
				"supported, defaulting to /usr/share/zoneinfo" << frg::endlog;
			tzdir = "/usr/share/zoneinfo";
		}

		path += tzdir;
		path += "/";
		path += tz;
	}

	return path;
}

bool parse_tzfile(const char *tz) {
	frg::string<MemoryAllocator> path = parse_tzfile_path(tz);

	// Check if file exists, otherwise fallback to the default.
	if (!mlibc::sys_stat) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"cannot proceed without sys_stat");
	}
	struct stat info;
	if (mlibc::sys_stat(mlibc::fsfd_target::path, -1, path.data(), 0, &info))
		return true;

	// FIXME: Make this fallible so the above check is not needed.
	file_window window {path.data()};

	// TODO(geert): we can probably cache this somehow
	tzfile tzfile_time;
	memcpy(&tzfile_time, reinterpret_cast<char *>(window.get()), sizeof(tzfile));
	tzfile_time.tzh_ttisgmtcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisgmtcnt);
	tzfile_time.tzh_ttisstdcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisstdcnt);
	tzfile_time.tzh_leapcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_leapcnt);
	tzfile_time.tzh_timecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_timecnt);
	tzfile_time.tzh_typecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_typecnt);
	tzfile_time.tzh_charcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_charcnt);

	if (tzfile_time.magic[0] != 'T' || tzfile_time.magic[1] != 'Z' || tzfile_time.magic[2] != 'i'
			|| tzfile_time.magic[3] != 'f') {
		mlibc::infoLogger() << "mlibc: " << path << " is not a valid TZinfo file" << frg::endlog;
		return true;
	}

	if (tzfile_time.version != '\0' && tzfile_time.version != '2' && tzfile_time.version != '3') {
		mlibc::infoLogger() << "mlibc: " << path << " has an invalid TZinfo version"
				<< frg::endlog;
		return true;
	}

	// There should be at least one entry in the ttinfo table.
	if (!tzfile_time.tzh_typecnt)
		return true;

	char *abbrevs = reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
		+ tzfile_time.tzh_timecnt * sizeof(int32_t)
		+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
		+ tzfile_time.tzh_typecnt * sizeof(struct ttinfo);
	bool found_std = false;
	bool found_dst = false;
	// start from the last ttinfo entry, this matches the behaviour of glibc and musl
	for (int i = tzfile_time.tzh_typecnt; i > 0; i--) {
		ttinfo time_info;
		memcpy(&time_info, reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
				+ tzfile_time.tzh_timecnt * sizeof(int32_t)
				+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
				+ i * sizeof(ttinfo), sizeof(ttinfo));
		time_info.tt_gmtoff = mlibc::bit_util<uint32_t>::byteswap(time_info.tt_gmtoff);
		if (!time_info.tt_isdst && !found_std) {
			tzname[0] = abbrevs + time_info.tt_abbrind;
			timezone = -time_info.tt_gmtoff;
			found_std = true;
		}
		if (time_info.tt_isdst && !found_dst) {
			tzname[1] = abbrevs + time_info.tt_abbrind;
			timezone = -time_info.tt_gmtoff;
			daylight = 1;
			found_dst = true;
		}
		if (found_std && found_dst)
			break;
	}

	rules[0].type = TZFILE;
	rules[1].type = TZFILE;

	return false;
}

// Assumes __time_lock is taken
// TODO(geert): this function doesn't properly handle the case where
// information might be missing from the tzinfo file
void do_tzset(void) {
	const char *tz = getenv("TZ");
	if (tz == nullptr)
		tz = "/etc/localtime";
	if (*tz == '\0')
		tz = "UTC0";

	size_t tz_name_max = TZNAME_MAX;
#if __MLIBC_POSIX_OPTION
	if (long sc_tz_name_max = sysconf(_SC_TZNAME_MAX); sc_tz_name_max > TZNAME_MAX)
		tz_name_max = static_cast<size_t>(sc_tz_name_max);
#endif

	// 1 byte for null
	char *tz_name = (char *) malloc(tz_name_max + 1);
	char *tz_name_dst = (char *) malloc(tz_name_max + 1);
	memset(tz_name, 0, tz_name_max + 1);
	memset(tz_name_dst, 0, tz_name_max + 1);

	// Reset daylight in case the TZ environment variable changed.
	daylight = 0;

	if (!parse_tz(tz, tz_name, tz_name_dst, tz_name_max)) {
		tzname[0] = tz_name;
		tzname[1] = tz_name_dst;
		return;
	}

	// Try parsing as a geographic timezone.
	if (parse_tzfile(tz)) {
		// This should always succeed.
		__ensure(!parse_tz("UTC0", tz_name, tz_name_dst, tz_name_max));
		tzname[0] = tz_name;
		tzname[1] = tz_name_dst;
	}
}

} // namespace

void tzset(void) {
	frg::unique_lock<FutexLock> lock(__time_lock);
	do_tzset();
}

// POSIX extensions.

int nanosleep(const struct timespec *req, struct timespec *) {
	if (req->tv_sec < 0 || req->tv_nsec > 999999999 || req->tv_nsec < 0) {
		errno = EINVAL;
		return -1;
	}

	if(!mlibc::sys_sleep) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}

	struct timespec tmp = *req;

	int e = mlibc::sys_sleep(&tmp.tv_sec, &tmp.tv_nsec);
	if (!e) {
		return 0;
	} else {
		errno = e;
		return -1;
	}
}

int clock_getres(clockid_t clockid, struct timespec *res) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_clock_getres, -1);
	if(int e = mlibc::sys_clock_getres(clockid, &res->tv_sec, &res->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int clock_gettime(clockid_t clock, struct timespec *time) {
	if(int e = mlibc::sys_clock_get(clock, &time->tv_sec, &time->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int clock_nanosleep(clockid_t clockid, int, const struct timespec *req, struct timespec *) {
	mlibc::infoLogger() << "clock_nanosleep is implemented as nanosleep!" << frg::endlog;
	__ensure(clockid == CLOCK_REALTIME || clockid == CLOCK_MONOTONIC);
	return nanosleep(req, nullptr);
}

int clock_settime(clockid_t clock, const struct timespec *time) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_clock_set, -1);
	if(int e = mlibc::sys_clock_set(clock, time->tv_sec, time->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

time_t time(time_t *out) {
	time_t secs;
	long nanos;
	if(int e = mlibc::sys_clock_get(CLOCK_REALTIME, &secs, &nanos); e) {
		errno = e;
		return (time_t)-1;
	}
	if(out)
		*out = secs;
	return secs;
}

namespace {

void civil_from_days(time_t days_since_epoch, int *year, unsigned int *month, unsigned int *day) {
	time_t time = days_since_epoch + 719468;
	int era = (time >= 0 ? time : time - 146096) / 146097;
	unsigned int doe = static_cast<unsigned int>(time - era * 146097);
	unsigned int yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;
	int y = static_cast<int>(yoe) + era * 400;
	unsigned int doy = doe - (365*yoe + yoe/4 - yoe/100);
	unsigned int mp = (5*doy + 2)/153;
	unsigned int d = doy - (153*mp+2)/5 + 1;
	unsigned int m = mp + (mp < 10 ? 3 : -9);

	*year = y + (m <= 2);
	*month = m;
	*day = d;
}

void weekday_from_days(time_t days_since_epoch, unsigned int *weekday) {
	*weekday = static_cast<unsigned int>(days_since_epoch >= -4 ?
			(days_since_epoch+4) % 7 : (days_since_epoch+5) % 7 + 6);
}

void yearday_from_date(unsigned int year, unsigned int month, unsigned int day, unsigned int *yday) {
	unsigned int n1 = 275 * month / 9;
	unsigned int n2 = (month + 9) / 12;
	unsigned int n3 = (1 + (year - 4 * year / 4 + 2) / 3);
	*yday = n1 - (n2 * n3) + day - 30;
}

static bool is_leap_year(int year) {
	return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// Given a rule and a year, compute the time of the transition in seconds since the epoch.
// TODO: Take into account the time of day when the transition occurs
time_t time_from_rule(const Rule &rule, int year) {
	if (rule.type == JULIAN_DAY) {
		// Jn: Julian day, ignoring Feb 29
		uint16_t day = rule.day - 1;
		if (is_leap_year(year) && day >= 60)
			day = rule.day;

		struct tm t = {};
		t.tm_year = year - 1900;
		t.tm_yday = day;
		return mktime(&t);
	} else if (rule.type == DAY_OF_YEAR) {
		// n: zero-based day of year, including Feb 29 in leap years
		struct tm t = {};
		t.tm_year = year - 1900;
		t.tm_yday = rule.day;
		return mktime(&t);
	} else if (rule.type == MONTH_NTH_DAY_OF_WEEK) {
		// Mm.n.d: Month, week, weekday (month 1-12, week 1-5, weekday 0=Sun)

		// Find the first day of the month
		struct tm t = {};
		t.tm_year = year - 1900;
		t.tm_mon = rule.month - 1;
		t.tm_mday = 1;
		mktime(&t);

		int first_wday = t.tm_wday;
		int day = 1 + ((7 + rule.day - first_wday) % 7) + (rule.week - 1) * 7;
		// If week==5, but that day is past the end of the month, go back by 7 days
		t.tm_mday = day;
		mktime(&t);
		if (rule.week == 5 && t.tm_mon != rule.month - 1)
			day -= 7;

		t.tm_year = year - 1900;
		t.tm_mon = rule.month - 1;
		t.tm_mday = day;
		t.tm_hour = 0;
		t.tm_min = 0;
		t.tm_sec = 0;
		return mktime(&t);
	} else {
		__ensure(!"Invalid rule type");
		__builtin_unreachable();
	}
}

// Assumes TZ environment variable rules are used, not TZFILE.
bool is_in_dst(time_t unix_gmt) {
	if (rules[0].type == TZFILE)
	    __ensure(!"is_in_dst() called with invalid rules");

	int year;
	unsigned int _month;
	unsigned int _day;
	civil_from_days(unix_gmt / (60 * 60 * 24), &year, &_month, &_day);

	// Get the start and end transition days of the year
	int start_time = time_from_rule(rules[0], year);
	int end_time = time_from_rule(rules[1], year);

	// Check if the unix_gmt falls within the DST period
	if (start_time <= end_time) {
		return unix_gmt >= start_time && unix_gmt < end_time;
	} else {
		// DST period wraps around the year end
		return unix_gmt >= start_time || unix_gmt < end_time;
	}
}

int unix_local_from_gmt_tzfile(time_t unix_gmt, time_t *offset, bool *dst, char **tm_zone) {
	const char *tz = getenv("TZ");

	if (!tz || *tz == '\0')
		tz = "/etc/localtime";

	frg::string<MemoryAllocator> path = parse_tzfile_path(tz);

	// Check if file exists
	if (!mlibc::sys_stat) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"cannot proceed without sys_stat");
	}
	struct stat info;
	if (mlibc::sys_stat(mlibc::fsfd_target::path, -1, path.data(), 0, &info))
		return -1;

	// FIXME: Make this fallible so the above check is not needed.
	file_window window {path.data()};

	// TODO(geert): we can probably cache this somehow
	tzfile tzfile_time;
	memcpy(&tzfile_time, reinterpret_cast<char *>(window.get()), sizeof(tzfile));
	tzfile_time.tzh_ttisgmtcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisgmtcnt);
	tzfile_time.tzh_ttisstdcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_ttisstdcnt);
	tzfile_time.tzh_leapcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_leapcnt);
	tzfile_time.tzh_timecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_timecnt);
	tzfile_time.tzh_typecnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_typecnt);
	tzfile_time.tzh_charcnt = mlibc::bit_util<uint32_t>::byteswap(tzfile_time.tzh_charcnt);

	if (tzfile_time.magic[0] != 'T' || tzfile_time.magic[1] != 'Z' || tzfile_time.magic[2] != 'i'
			|| tzfile_time.magic[3] != 'f') {
		mlibc::infoLogger() << "mlibc: " << path << " is not a valid TZinfo file" << frg::endlog;
		return -1;
	}

	if (tzfile_time.version != '\0' && tzfile_time.version != '2' && tzfile_time.version != '3') {
		mlibc::infoLogger() << "mlibc: " << path << " has an invalid TZinfo version"
				<< frg::endlog;
		return -1;
	}

	int index = -1;
	for (size_t i = 0; i < tzfile_time.tzh_timecnt; i++) {
		int32_t ttime;
		memcpy(&ttime, reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
				+ i * sizeof(int32_t), sizeof(int32_t));
		ttime = mlibc::bit_util<uint32_t>::byteswap(ttime);
		// If we are before the first transition, the format dicates that
		// the first ttinfo entry should be used (and not the ttinfo entry pointed
		// to by the first transition time).
		if (i && ttime > unix_gmt) {
			index = i - 1;
			break;
		}
	}

	// The format dictates that if no transition is applicable,
	// the first entry in the file is chosen.
	uint8_t ttinfo_index = 0;
	if (index >= 0) {
		memcpy(&ttinfo_index, reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
				+ tzfile_time.tzh_timecnt * sizeof(int32_t)
				+ index * sizeof(uint8_t), sizeof(uint8_t));
	}

	// There should be at least one entry in the ttinfo table.
	// TODO: If there is not, we might want to fall back to UTC, no DST (?).
	__ensure(tzfile_time.tzh_typecnt);

	ttinfo time_info;
	memcpy(&time_info, reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
			+ tzfile_time.tzh_timecnt * sizeof(int32_t)
			+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
			+ ttinfo_index * sizeof(ttinfo), sizeof(ttinfo));
	time_info.tt_gmtoff = mlibc::bit_util<uint32_t>::byteswap(time_info.tt_gmtoff);

	char *abbrevs = reinterpret_cast<char *>(window.get()) + sizeof(tzfile)
		+ tzfile_time.tzh_timecnt * sizeof(int32_t)
		+ tzfile_time.tzh_timecnt * sizeof(uint8_t)
		+ tzfile_time.tzh_typecnt * sizeof(struct ttinfo);

	*offset = time_info.tt_gmtoff;
	*dst = time_info.tt_isdst;
	*tm_zone = abbrevs + time_info.tt_abbrind;
	return 0;
}

// Looks up the local time rules for a given
// UNIX GMT timestamp (seconds since 1970 GMT, ignoring leap seconds).
// This function assumes the __time_lock has been taken
int unix_local_from_gmt(time_t unix_gmt, time_t *offset, bool *dst, char **tm_zone) {
	do_tzset();

	if (daylight && rules[0].type == TZFILE)
		return unix_local_from_gmt_tzfile(unix_gmt, offset, dst, tm_zone);

	if (daylight && is_in_dst(unix_gmt)) {
		*offset = tt_infos[1].tt_gmtoff;
		*dst = true;
		*tm_zone = tzname[1];
		return 0;
	}

	*offset = -timezone;
	*dst = false;
	*tm_zone = tzname[0];
	return 0;
}

} //anonymous namespace

struct tm *gmtime_r(const time_t *unix_gmt, struct tm *res) {
	int year;
	unsigned int month;
	unsigned int day;
	unsigned int weekday;
	unsigned int yday;

	time_t unix_local = *unix_gmt;

	int days_since_epoch = unix_local / (60*60*24);
	civil_from_days(days_since_epoch, &year, &month, &day);
	weekday_from_days(days_since_epoch, &weekday);
	yearday_from_date(year, month, day, &yday);

	res->tm_sec = unix_local % 60;
	res->tm_min = (unix_local / 60) % 60;
	res->tm_hour = (unix_local / (60*60)) % 24;
	res->tm_mday = day;
	res->tm_mon = month - 1;
	res->tm_year = year - 1900;
	res->tm_wday = weekday;
	res->tm_yday = yday - 1;
	res->tm_isdst = -1;
	res->tm_zone = __utc;
	res->tm_gmtoff = 0;

	return res;
}

struct tm *localtime_r(const time_t *unix_gmt, struct tm *res) {
	int year;
	unsigned int month;
	unsigned int day;
	unsigned int weekday;
	unsigned int yday;

	time_t offset = 0;
	bool dst;
	char *tm_zone;
	frg::unique_lock<FutexLock> lock(__time_lock);
	// TODO: Set errno if the conversion fails.
	if(unix_local_from_gmt(*unix_gmt, &offset, &dst, &tm_zone)) {
		__ensure(!"Error parsing /etc/localtime");
		__builtin_unreachable();
	}
	time_t unix_local = *unix_gmt + offset;

	int days_since_epoch = unix_local / (60*60*24);
	civil_from_days(days_since_epoch, &year, &month, &day);
	weekday_from_days(days_since_epoch, &weekday);
	yearday_from_date(year, month, day, &yday);

	res->tm_sec = unix_local % 60;
	res->tm_min = (unix_local / 60) % 60;
	res->tm_hour = (unix_local / (60*60)) % 24;
	res->tm_mday = day;
	res->tm_mon = month - 1;
	res->tm_year = year - 1900;
	res->tm_wday = weekday;
	res->tm_yday = yday - 1;
	res->tm_isdst = dst;
	res->tm_zone = tm_zone;
	res->tm_gmtoff = offset;

	return res;
}

// This implementation of asctime_r is taken from sortix
char *asctime_r(const struct tm *tm, char *buf) {
	static char weekday_names[7][4] =
		{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	static char month_names[12][4] =
		{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
		  "Nov", "Dec" };
	sprintf(buf, "%.3s %.3s%3d %.2d:%.2d%.2d %d\n",
				 weekday_names[tm->tm_wday],
				 month_names[tm->tm_mon],
				 tm->tm_mday,
				 tm->tm_hour,
				 tm->tm_min,
				 tm->tm_sec,
				 tm->tm_year + 1900);
	return buf;
}

char *ctime_r(const time_t *clock, char *buf) {
	return asctime_r(localtime(clock), buf);
}

time_t timelocal(struct tm *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

constexpr static int days_from_civil(int y, unsigned m, unsigned d) noexcept {
	y -= m <= 2;
	const int era = (y >= 0 ? y : y - 399) / 400;
	const unsigned yoe = static_cast<unsigned>(y - era * 400); // [0, 399]
	const unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1; // [0, 365]
	const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy; // [0, 146096]
	return era * 146097 + static_cast<int>(doe) - 719468;
}

time_t timegm(struct tm *tm) {
	time_t year = tm->tm_year + 1900;
	time_t month = tm->tm_mon + 1;
	time_t days = days_from_civil(year, month, tm->tm_mday);
	time_t secs = (days * 86400) + (tm->tm_hour * 60 * 60) + (tm->tm_min * 60) + tm->tm_sec;
	return secs;
}
