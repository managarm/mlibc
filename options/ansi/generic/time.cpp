#include <algorithm>
#include <array>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <limits>
#include <stdint.h>
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
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/lock.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/bitutil.hpp>
#include <mlibc/strings.hpp>
#include <mlibc/time.hpp>

#include <frg/mutex.hpp>

// The DST rules to use if TZ has no rules and we can't load posixinfo.
// POSIX does not specify the default DST rules, for historical reasons
// America/New_York is a common default.
#define TZ_DEFAULT_RULE_STRING ",M3.2.0,M11.1.0"

namespace {

const char __utc[] = "UTC";

constexpr size_t tznameNormal = 0;
constexpr size_t tznameDST = 1;

frg::string<MemoryAllocator> tznameStorage[2] = { {getAllocator()}, {getAllocator()} };
frg::string<MemoryAllocator> tmZoneStorage {getAllocator()};

} // namespace

// Variables defined by POSIX.
int daylight;
long timezone;
// [0] holds normal time, [1] holds DST
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

size_t strftime(
    char *__restrict dest,
    size_t max_size,
    const char *__restrict format,
    const struct tm *__restrict tm
) {
	return mlibc::strftime(dest, max_size, format, tm, mlibc::getActiveLocale());
}

size_t wcsftime(
    wchar_t *__restrict dest,
    size_t max_size,
    const wchar_t *__restrict format,
    const struct tm *__restrict tm
) {
	return mlibc::strftime(dest, max_size, format, tm, mlibc::getActiveLocale());
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
		if (tzn_len > tz_name_max)
			return true;
		if (*tz == '\0')
			break;

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
		if (tzn_len_dst > tz_name_max)
			return false;
		if (*tz == '\0')
			break;

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

struct tzfile_data {
	tzfile header;
	const char *data;
	size_t time_size;
};

bool read_tzfile_header(const char *data, tzfile *header) {
	memcpy(header, data, sizeof(tzfile));
	header->tzh_ttisgmtcnt = mlibc::bit_util<uint32_t>::be_to_host(header->tzh_ttisgmtcnt);
	header->tzh_ttisstdcnt = mlibc::bit_util<uint32_t>::be_to_host(header->tzh_ttisstdcnt);
	header->tzh_leapcnt = mlibc::bit_util<uint32_t>::be_to_host(header->tzh_leapcnt);
	header->tzh_timecnt = mlibc::bit_util<uint32_t>::be_to_host(header->tzh_timecnt);
	header->tzh_typecnt = mlibc::bit_util<uint32_t>::be_to_host(header->tzh_typecnt);
	header->tzh_charcnt = mlibc::bit_util<uint32_t>::be_to_host(header->tzh_charcnt);

	return header->magic[0] == 'T' && header->magic[1] == 'Z' && header->magic[2] == 'i'
			&& header->magic[3] == 'f' && (header->version == '\0' || header->version == '2'
					|| header->version == '3');
}

size_t tzfile_block_size(const tzfile &header, size_t time_size) {
	return header.tzh_timecnt * time_size + header.tzh_timecnt
			+ header.tzh_typecnt * sizeof(ttinfo) + header.tzh_charcnt
			+ header.tzh_leapcnt * (time_size + sizeof(uint32_t))
			+ header.tzh_ttisstdcnt + header.tzh_ttisgmtcnt;
}

bool get_tzfile_data(const char *file, tzfile_data *data) {
	tzfile header;
	if(!read_tzfile_header(file, &header))
		return false;

	const char *block = file + sizeof(tzfile);
	size_t time_size = sizeof(int32_t);
	if(header.version != '\0') {
		block += tzfile_block_size(header, time_size);
		if(!read_tzfile_header(block, &header))
			return false;
		block += sizeof(tzfile);
		time_size = sizeof(int64_t);
	}

	data->header = header;
	data->data = block;
	data->time_size = time_size;
	return true;
}

const char *tzfile_transition_types(const tzfile_data &data) {
	return data.data + data.header.tzh_timecnt * data.time_size;
}

const char *tzfile_ttinfos(const tzfile_data &data) {
	return tzfile_transition_types(data) + data.header.tzh_timecnt;
}

const char *tzfile_abbrevs(const tzfile_data &data) {
	return tzfile_ttinfos(data) + data.header.tzh_typecnt * sizeof(ttinfo);
}

ttinfo get_tzfile_ttinfo(const tzfile_data &data, size_t index) {
	ttinfo info;
	memcpy(&info, tzfile_ttinfos(data) + index * sizeof(ttinfo), sizeof(ttinfo));
	uint32_t offset;
	memcpy(&offset, &info.tt_gmtoff, sizeof(offset));
	offset = mlibc::bit_util<uint32_t>::be_to_host(offset);
	memcpy(&info.tt_gmtoff, &offset, sizeof(offset));
	return info;
}

int64_t get_tzfile_transition_time(const tzfile_data &data, size_t index) {
	const char *ptr = data.data + index * data.time_size;
	if(data.time_size == sizeof(int32_t)) {
		uint32_t value;
		memcpy(&value, ptr, sizeof(value));
		value = mlibc::bit_util<uint32_t>::be_to_host(value);
		int32_t signed_value;
		memcpy(&signed_value, &value, sizeof(signed_value));
		return signed_value;
	} else {
		uint64_t value;
		memcpy(&value, ptr, sizeof(value));
		value = mlibc::bit_util<uint64_t>::be_to_host(value);
		int64_t signed_value;
		memcpy(&signed_value, &value, sizeof(signed_value));
		return signed_value;
	}
}

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
	if constexpr (!mlibc::IsImplemented<Stat>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"cannot proceed without sys_stat");
	}
	struct stat info;
	if (mlibc::sysdep_or_panic<Stat>(mlibc::fsfd_target::path, -1, path.data(), 0, &info))
		return true;

	// FIXME: Make this fallible so the above check is not needed.
	file_window window {path.data()};

	tzfile_data data;
	if(!get_tzfile_data(reinterpret_cast<char *>(window.get()), &data)) {
		mlibc::infoLogger() << "mlibc: " << path << " is not a valid TZinfo file" << frg::endlog;
		return true;
	}

	// There should be at least one entry in the ttinfo table.
	if (!data.header.tzh_typecnt)
		return true;

	const char *abbrevs = tzfile_abbrevs(data);
	bool found_std = false;
	bool found_dst = false;
	// start from the last ttinfo entry, this matches the behaviour of glibc and musl
	for(size_t i = data.header.tzh_typecnt; i > 0; i--) {
		ttinfo time_info = get_tzfile_ttinfo(data, i - 1);
		if(time_info.tt_abbrind >= data.header.tzh_charcnt)
			return true;
		if (!time_info.tt_isdst && !found_std) {
			tznameStorage[tznameNormal] = {abbrevs + time_info.tt_abbrind, getAllocator()};
			tzname[tznameNormal] = tznameStorage[tznameNormal].data();
			timezone = -time_info.tt_gmtoff;
			found_std = true;
		}
		if (time_info.tt_isdst && !found_dst) {
			tznameStorage[tznameDST] = {abbrevs + time_info.tt_abbrind, getAllocator()};
			tzname[tznameDST] = tznameStorage[tznameDST].data();
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
		tzname[tznameNormal] = tz_name;
		tzname[tznameDST] = tz_name_dst;
		return;
	}

	// Try parsing as a geographic timezone.
	if (parse_tzfile(tz)) {
		// This should always succeed.
		__ensure(!parse_tz("UTC0", tz_name, tz_name_dst, tz_name_max));
		tzname[tznameNormal] = tz_name;
		tzname[tznameDST] = tz_name_dst;
	}
}

} // namespace

void tzset(void) {
	frg::unique_lock<FutexLock> lock(__time_lock);
	do_tzset();
}

// POSIX extensions.

int nanosleep(const struct timespec *req, struct timespec *rem) {
	if (req->tv_sec < 0 || req->tv_nsec > 999999999 || req->tv_nsec < 0) {
		errno = EINVAL;
		return -1;
	}

	if constexpr (!mlibc::IsImplemented<Sleep>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}

	struct timespec tmp = *req;

	int e = mlibc::sysdep_or_panic<Sleep>(&tmp.tv_sec, &tmp.tv_nsec);
	if (!e)
		return 0;
	else if (e == EINTR && rem)
		*rem = tmp;

	errno = e;
	return -1;
}

int clock_getres(clockid_t clockid, struct timespec *res) {
	if(int e = mlibc::sysdep_or_enosys<ClockGetres>(clockid, &res->tv_sec, &res->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int clock_gettime(clockid_t clock, struct timespec *time) {
	if(int e = mlibc::sysdep<ClockGet>(clock, &time->tv_sec, &time->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int clock_nanosleep(clockid_t clockid, int flags, const struct timespec *req, struct timespec *rem) {
	__ensure(clockid == CLOCK_REALTIME || clockid == CLOCK_MONOTONIC);

	if (flags & TIMER_ABSTIME) {
		time_t secs = 0;
		long nanos = 0;
		if(int e = mlibc::sysdep<ClockGet>(clockid, &secs, &nanos); e) {
			errno = e;
			return -1;
		}

		struct timespec relativeTime;

		if (secs > req->tv_sec)
			return 0;
		else if (secs == req->tv_sec && nanos >= req->tv_nsec)
			return 0;
		else {
			relativeTime.tv_sec = req->tv_sec - secs;
			relativeTime.tv_nsec = req->tv_nsec - nanos;
			if (relativeTime.tv_nsec < 0) {
				relativeTime.tv_sec -= 1;
				relativeTime.tv_nsec += 1e9;
			}
		}

		return nanosleep(&relativeTime, rem);
	}

	return nanosleep(req, rem);
}

int clock_settime(clockid_t clock, const struct timespec *time) {
	if(int e = mlibc::sysdep_or_enosys<ClockSet>(clock, time->tv_sec, time->tv_nsec); e) {
		errno = e;
		return -1;
	}
	return 0;
}

time_t time(time_t *out) {
	time_t secs;
	long nanos;
	if(int e = mlibc::sysdep<ClockGet>(CLOCK_REALTIME, &secs, &nanos); e) {
		errno = e;
		return (time_t)-1;
	}
	if(out)
		*out = secs;
	return secs;
}

namespace {

constexpr static int64_t days_from_civil(int64_t year, unsigned int month, unsigned int day) noexcept {
	int64_t y = year;
	y -= month <= 2;
	const int64_t era = (y >= 0 ? y : y - 399) / 400;
	const unsigned int yoe = static_cast<unsigned int>(y - era * 400); // [0, 399]
	const unsigned int doy = (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;
	const unsigned int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	return era * 146097 + static_cast<int64_t>(doe) - 719468;
}

bool civil_from_days(time_t days_since_epoch, int *year, unsigned int *month, unsigned int *day) {
	int64_t time = static_cast<int64_t>(days_since_epoch) + 719468;
	int64_t era = (time >= 0 ? time : time - 146096) / 146097;
	unsigned int doe = static_cast<unsigned int>(time - era * 146097);
	unsigned int yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;
	int64_t y = static_cast<int64_t>(yoe) + era * 400;
	unsigned int doy = doe - (365*yoe + yoe/4 - yoe/100);
	unsigned int mp = (5*doy + 2)/153;
	unsigned int d = doy - (153*mp+2)/5 + 1;
	unsigned int m = mp + (mp < 10 ? 3 : -9);

	y += m <= 2;
	if(y - 1900 < INT_MIN || y - 1900 > INT_MAX)
		return false;
	*year = static_cast<int>(y);
	*month = m;
	*day = d;
	return true;
}

void weekday_from_days(time_t days_since_epoch, unsigned int *weekday) {
	*weekday = static_cast<unsigned int>(days_since_epoch >= -4 ?
			(days_since_epoch+4) % 7 : (days_since_epoch+5) % 7 + 6);
}

static bool is_leap_year(int year) {
	return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// Returns the one-based day of the year.
void yearday_from_date(int year, unsigned int month, unsigned int day, unsigned int *yday) {
	static const unsigned int days_before_month[12] = {
		0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
	};

	*yday = days_before_month[month - 1] + day;
	if(month > 2 && is_leap_year(year))
		(*yday)++;
}

// Given a rule and a year, compute the UTC time of the transition.
int64_t time_from_rule(const Rule &rule, int year, time_t offset_before_transition) {
	int64_t days;
	if (rule.type == JULIAN_DAY) {
		// Jn: Julian day, ignoring Feb 29
		uint16_t day = rule.day - 1;
		if (is_leap_year(year) && day >= 59)
			day++;
		days = days_from_civil(year, 1, 1) + day;
	} else if (rule.type == DAY_OF_YEAR) {
		// n: zero-based day of year, including Feb 29 in leap years
		days = days_from_civil(year, 1, 1) + rule.day;
	} else if (rule.type == MONTH_NTH_DAY_OF_WEEK) {
		// Mm.n.d: Month, week, weekday (month 1-12, week 1-5, weekday 0=Sun)
		int64_t first_day = days_from_civil(year, rule.month, 1);
		unsigned int first_wday;
		weekday_from_days(first_day, &first_wday);
		int day = 1 + ((7 + rule.day - first_wday) % 7) + (rule.week - 1) * 7;
		// If week==5, but that day is past the end of the month, go back by 7 days
		int64_t next_month = days_from_civil(year + (rule.month == 12), rule.month % 12 + 1, 1);
		if (rule.week == 5 && first_day + day - 1 >= next_month)
			day -= 7;
		days = first_day + day - 1;
	} else {
		__ensure(!"Invalid rule type");
		__builtin_unreachable();
	}

	// POSIX transition times are expressed in local time before the transition.
	return days * (60 * 60 * 24) + rule.time - offset_before_transition;
}

// Assumes TZ environment variable rules are used, not TZFILE.
bool is_in_dst(time_t unix_gmt) {
	if (rules[0].type == TZFILE)
	    __ensure(!"is_in_dst() called with invalid rules");

	int year;
	unsigned int _month;
	unsigned int _day;
	time_t days_since_epoch = unix_gmt / (60 * 60 * 24);
	if(unix_gmt % (60 * 60 * 24) < 0)
		days_since_epoch--;
	if(!civil_from_days(days_since_epoch, &year, &_month, &_day))
		return false;

	// Get the start and end transition days of the year
	int64_t start_time = time_from_rule(rules[0], year, tt_infos[tznameNormal].tt_gmtoff);
	int64_t end_time = time_from_rule(rules[1], year, tt_infos[tznameDST].tt_gmtoff);
	int64_t unix_time = unix_gmt;

	// Check if the unix_gmt falls within the DST period
	if (start_time <= end_time) {
		return unix_time >= start_time && unix_time < end_time;
	} else {
		// DST period wraps around the year end
		return unix_time >= start_time || unix_time < end_time;
	}
}

int unix_local_from_gmt_tzfile(time_t unix_gmt, time_t *offset, bool *dst, frg::string<MemoryAllocator> &tm_zone) {
	const char *tz = getenv("TZ");

	if (!tz || *tz == '\0')
		tz = "/etc/localtime";

	frg::string<MemoryAllocator> path = parse_tzfile_path(tz);

	// Check if file exists
	if constexpr (!mlibc::IsImplemented<Stat>) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"cannot proceed without sys_stat");
	}
	struct stat info;
	if (mlibc::sysdep_or_panic<Stat>(mlibc::fsfd_target::path, -1, path.data(), 0, &info))
		return -1;

	// FIXME: Make this fallible so the above check is not needed.
	file_window window {path.data()};

	tzfile_data data;
	if(!get_tzfile_data(reinterpret_cast<char *>(window.get()), &data)) {
		mlibc::infoLogger() << "mlibc: " << path << " is not a valid TZinfo file" << frg::endlog;
		return -1;
	}

	// Before the first transition, TZif requires type zero. Afterwards use the
	// last transition at or before the requested instant.
	uint8_t ttinfo_index = 0;
	for(size_t i = 0; i < data.header.tzh_timecnt; i++) {
		if(get_tzfile_transition_time(data, i) > unix_gmt)
			break;
		ttinfo_index = static_cast<uint8_t>(tzfile_transition_types(data)[i]);
	}

	if(!data.header.tzh_typecnt || ttinfo_index >= data.header.tzh_typecnt)
		return -1;
	ttinfo time_info = get_tzfile_ttinfo(data, ttinfo_index);
	if(time_info.tt_abbrind >= data.header.tzh_charcnt)
		return -1;

	*offset = time_info.tt_gmtoff;
	*dst = time_info.tt_isdst;
	tm_zone = {tzfile_abbrevs(data) + time_info.tt_abbrind, getAllocator()};
	return 0;
}

// Looks up the local time rules for a given
// UNIX GMT timestamp (seconds since 1970 GMT, ignoring leap seconds).
// This function assumes the __time_lock has been taken
int unix_local_from_gmt(time_t unix_gmt, time_t *offset, bool *dst, char **tm_zone) {
	do_tzset();

	if (rules[0].type == TZFILE) {
		int ret = unix_local_from_gmt_tzfile(unix_gmt, offset, dst, tmZoneStorage);
		if (ret == 0)
			*tm_zone = tmZoneStorage.data();
		return ret;
	}

	if (daylight && is_in_dst(unix_gmt)) {
		*offset = tt_infos[tznameDST].tt_gmtoff;
		*dst = true;
		*tm_zone = tzname[tznameDST];
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

	// Division truncates towards zero and the remainder keeps the sign of the
	// dividend, so a time before the epoch has to be carried into the previous
	// day to leave a non-negative time of day.
	time_t days_since_epoch = unix_local / (60*60*24);
	time_t secs_of_day = unix_local % (60*60*24);
	if(secs_of_day < 0) {
		secs_of_day += 60*60*24;
		days_since_epoch--;
	}

	if(!civil_from_days(days_since_epoch, &year, &month, &day)) {
		errno = EOVERFLOW;
		return nullptr;
	}
	weekday_from_days(days_since_epoch, &weekday);
	yearday_from_date(year, month, day, &yday);

	res->tm_sec = secs_of_day % 60;
	res->tm_min = (secs_of_day / 60) % 60;
	res->tm_hour = secs_of_day / (60*60);
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
	time_t unix_local;
	if(__builtin_add_overflow(*unix_gmt, offset, &unix_local)) {
		errno = EOVERFLOW;
		return nullptr;
	}

	// See the comment in gmtime_r().
	time_t days_since_epoch = unix_local / (60*60*24);
	time_t secs_of_day = unix_local % (60*60*24);
	if(secs_of_day < 0) {
		secs_of_day += 60*60*24;
		days_since_epoch--;
	}

	if(!civil_from_days(days_since_epoch, &year, &month, &day)) {
		errno = EOVERFLOW;
		return nullptr;
	}
	weekday_from_days(days_since_epoch, &weekday);
	yearday_from_date(year, month, day, &yday);

	res->tm_sec = secs_of_day % 60;
	res->tm_min = (secs_of_day / 60) % 60;
	res->tm_hour = secs_of_day / (60*60);
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
	sprintf(buf, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
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

bool int64_to_time_t(int64_t value, time_t *result) {
	if constexpr (std::numeric_limits<time_t>::is_signed) {
		if(value < std::numeric_limits<time_t>::min() || value > std::numeric_limits<time_t>::max())
			return false;
	} else {
		if(value < 0 || static_cast<uint64_t>(value) > std::numeric_limits<time_t>::max())
			return false;
	}
	*result = static_cast<time_t>(value);
	return true;
}

bool timegm_to_timestamp(const struct tm *tm, time_t *result) {
	int64_t year = static_cast<int64_t>(tm->tm_year) + 1900;
	int64_t month = tm->tm_mon;
	int64_t year_adjustment = month / 12;
	month %= 12;
	if(month < 0) {
		month += 12;
		year_adjustment--;
	}
	year += year_adjustment;

	int64_t days = days_from_civil(year, static_cast<unsigned int>(month + 1), 1)
			+ static_cast<int64_t>(tm->tm_mday) - 1;
	int64_t seconds = static_cast<int64_t>(tm->tm_hour) * 60 * 60
			+ static_cast<int64_t>(tm->tm_min) * 60 + tm->tm_sec;
	int64_t timestamp;
	if(__builtin_mul_overflow(days, INT64_C(86400), &timestamp)
			|| __builtin_add_overflow(timestamp, seconds, &timestamp))
		return false;
	return int64_to_time_t(timestamp, result);
}

time_t timegm(struct tm *tm) {
	time_t timestamp;
	if(!timegm_to_timestamp(tm, &timestamp)) {
		errno = EOVERFLOW;
		return -1;
	}

	struct tm normalized;
	if(!gmtime_r(&timestamp, &normalized))
		return -1;
	normalized.tm_isdst = 0;
	*tm = normalized;
	return timestamp;
}
